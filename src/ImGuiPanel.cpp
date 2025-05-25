#include "ImGuiPanel.h"

ImGuiPanel::ImGuiPanel(int posX, int posY, int width, int height, float* data)
    : m_posX(posX), m_posY(posY), m_width(width), m_height(height), m_data(data) {}

void ImGuiPanel::Draw() {
    ImGui::SetNextWindowPos(ImVec2(m_posX, m_posY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_width, m_height), ImGuiCond_Always);
    ImGui::Begin("Example GUI");
    ImGui::Text("Example Text");
    ImGui::SliderFloat("Example Slider", m_data, 0.0f, 1.0f);
    ImGui::End();
}