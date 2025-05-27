#pragma once
#include "implot.h"
#include "imgui.h"
#include "ThreadSafeRingBuffer.h"
#include "Config.h"
#include "SensorPlot.h"

class ImPlotPanel {
private:
    int m_posX;
    int m_posY;
    int m_width;
    int m_height;
    
    float m_vertical_zoom;   // Overall panel zoom (affects height)
    float m_horizontal_zoom; // X-axis zoom (shared across plots)
    
    SensorPlot<gyroBufferSize> m_gyroPlot;
    SensorPlot<accelBufferSize> m_accelPlot;
    SensorPlot<magBufferSize> m_magPlot;

    static constexpr float min_zoom = 0.1f;
    static constexpr float max_zoom = 10.0f;

public:
    ImPlotPanel(int posX, int posY, int width, int height, 
                GyroBuffer& gyroBuffer_ref,
                AccelBuffer& accelBuffer_ref, 
                MagBuffer& magBuffer_ref,
                std::array<float, gyroBufferSize>& gyroTimeBuffer_ref,
                std::array<float, accelBufferSize>& accelTimeBuffer_ref,
                std::array<float, magBufferSize>& magTimeBuffer_ref
                );

    void Draw();
};