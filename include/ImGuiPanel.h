#pragma once
#include "imgui.h"

class ImGuiPanel {
private:
    int m_posX;
    int m_posY;
    int m_width;
    int m_height;
    float* m_data;

public:
    ImGuiPanel(int posX, int posY, int width, int height, float* data);
    void Draw();
};