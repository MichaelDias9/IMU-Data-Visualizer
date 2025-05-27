#pragma once
#include "ThreadSafeRingBuffer.h"
#include "Config.h"
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <functional>
#include "implot.h"

template <size_t Capacity>
class SensorPlot {
private:
    std::string m_name;
    ThreadSafeRingBuffer<Capacity>& m_data_buffer_ref;
    std::array<float, Capacity>& m_time_buffer_ref;
    mutable float m_y_min; // Track Y-axis limits
    mutable float m_y_max;
    static constexpr size_t MAX_PLOT_POINTS = 1000; // Downsampling threshold 

public:
    SensorPlot(const std::string name, ThreadSafeRingBuffer<Capacity>& buffer, std::array<float, Capacity>& timeBuffer)
        : m_name(name), m_data_buffer_ref(buffer), m_time_buffer_ref(timeBuffer),
          m_y_min(-1.1f), m_y_max(1.1f) {}

    void Draw(float height, float horizontal_zoom) const {
        if (ImPlot::BeginPlot(m_name.c_str(), ImVec2(-1, height))) {
            const float* xDataPtr = nullptr;
            const float* yDataPtr = nullptr;
            const float* zDataPtr = nullptr;
            m_data_buffer_ref.getRecentPointers(Capacity, &xDataPtr, &yDataPtr, &zDataPtr);

            
            if (xDataPtr && yDataPtr && zDataPtr) {
                float displayed_range = bufferSeconds / horizontal_zoom;

                // Configure X axis label formatter
                ImPlot::SetupAxisFormat(ImAxis_X1, TimeFormatter);

                // Configure axes
                ImPlot::SetupAxes("Time (s)", "Value");
                ImPlot::SetupAxisLimits(ImAxis_X1, -displayed_range, 0, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, m_y_min, m_y_max, ImGuiCond_Once);

                // Handle Y-axis zoom 
                if (ImPlot::IsPlotHovered() && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) {
                    float wheel = ImGui::GetIO().MouseWheel;
                    if (wheel != 0.0f) {
                        float delta = wheel * 0.1f; // Sensitivity adjustment
                        float center = (m_y_min + m_y_max) * 0.5f;
                        float range = m_y_max - m_y_min;
                        float new_range = range * (1.0f - delta);
                        new_range = std::clamp(new_range, 0.1f, 100.0f);
                        m_y_min = center - new_range * 0.5f;
                        m_y_max = center + new_range * 0.5f;
                    }
                }

                // Plot data 
                if (Capacity <= MAX_PLOT_POINTS) {
                    ImPlot::PlotLine("X", m_time_buffer_ref.data(), xDataPtr, Capacity);
                    ImPlot::PlotLine("Y", m_time_buffer_ref.data(), yDataPtr, Capacity);
                    ImPlot::PlotLine("Z", m_time_buffer_ref.data(), zDataPtr, Capacity);
                } else {
                    // Downsample by taking every nth point
                    size_t step = Capacity / MAX_PLOT_POINTS;
                    std::vector<float> sampledTime, sampledX, sampledY, sampledZ;
                    sampledTime.reserve(MAX_PLOT_POINTS);
                    sampledX.reserve(MAX_PLOT_POINTS);
                    sampledY.reserve(MAX_PLOT_POINTS);
                    sampledZ.reserve(MAX_PLOT_POINTS);
                    
                    for (size_t i = 0; i < Capacity; i += step) {
                        sampledTime.push_back(m_time_buffer_ref[i]);
                        sampledX.push_back(xDataPtr[i]);
                        sampledY.push_back(yDataPtr[i]);
                        sampledZ.push_back(zDataPtr[i]);
                    }
                    
                    ImPlot::PlotLine("X", sampledTime.data(), sampledX.data(), sampledTime.size());
                    ImPlot::PlotLine("Y", sampledTime.data(), sampledY.data(), sampledTime.size());
                    ImPlot::PlotLine("Z", sampledTime.data(), sampledZ.data(), sampledTime.size());                    
                }
            }
            ImPlot::EndPlot();
        }
    }

private:
    // Custom formatter function for time axis
    static int TimeFormatter(double value, char* buff, int size, void* data) {
        // Only show labels for whole seconds
        if (std::fmod(value, 1.0) == 0.0) {
            return snprintf(buff, size, "%.0fs", value);
        } else {
            // Return empty string for non-whole seconds
            buff[0] = '\0';
            return 0;
        }
    }
};