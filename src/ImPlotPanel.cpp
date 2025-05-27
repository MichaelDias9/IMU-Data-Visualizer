#include "ImPlotPanel.h"

ImPlotPanel::ImPlotPanel(int posX, int posY, int width, int height, 
                         GyroBuffer& gyroBuffer_ref,
                         AccelBuffer& accelBuffer_ref, 
                         MagBuffer& magBuffer_ref,
                         std::array<float, gyroBufferSize>& gyroTimeBuffer_ref,
                         std::array<float, accelBufferSize>& accelTimeBuffer_ref,
                         std::array<float, magBufferSize>& magTimeBuffer_ref)

    : m_posX(posX), m_posY(posY), m_width(width), m_height(height),
      m_vertical_zoom(1.0f), m_horizontal_zoom(1.0f),
      m_gyroPlot("Gyro", gyroBuffer_ref, gyroTimeBuffer_ref),
      m_accelPlot("Accel", accelBuffer_ref, accelTimeBuffer_ref),
      m_magPlot("Mag", magBuffer_ref, magTimeBuffer_ref) {}

void ImPlotPanel::Draw() {
    ImGui::SetNextWindowPos(ImVec2(m_posX, m_posY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_width, m_height), ImGuiCond_Always);
    ImGui::Begin("Sensor Data", nullptr, 
          ImGuiWindowFlags_NoMove | 
          ImGuiWindowFlags_NoResize | 
          ImGuiWindowFlags_NoCollapse |
          ImGuiWindowFlags_AlwaysVerticalScrollbar |
          ImGuiWindowFlags_NoScrollWithMouse
    );

    // Handle zoom inputs
    if (ImGui::IsWindowHovered()) {
        // Ctrl + Mouse Wheel: Vertical zoom (panel height)
        if (ImGui::GetIO().KeyCtrl) {
            m_vertical_zoom += ImGui::GetIO().MouseWheel * 0.1f;
            m_vertical_zoom = std::clamp(m_vertical_zoom, min_zoom, max_zoom);

        }
        // Shift + Mouse Wheel: Horizontal zoom (X-axis)
        else if (ImGui::GetIO().KeyShift) {
            m_horizontal_zoom += ImGui::GetIO().MouseWheel * 0.1f;
            m_horizontal_zoom = std::clamp(m_horizontal_zoom, min_zoom, max_zoom);

        }
    }

    // Display zoom info
    ImGui::Text("Panel Zoom: %.1fx (Ctrl+Wheel)\nTime Axis Zoom %.1fx (Shift+Wheel)", 
                m_vertical_zoom, m_horizontal_zoom);

    // Calculate plot heights with vertical zoom
    const float content_height = ImGui::GetContentRegionAvail().y;
    const float total_plots_height = content_height * m_vertical_zoom;
    const float plot_height = total_plots_height / 3.0f;

    // Draw plots with individual heights and shared horizontal zoom
    m_gyroPlot.Draw(plot_height, m_horizontal_zoom);
    ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y));
    m_accelPlot.Draw(plot_height, m_horizontal_zoom);
    ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y));
    m_magPlot.Draw(plot_height, m_horizontal_zoom);

    ImGui::End();
}