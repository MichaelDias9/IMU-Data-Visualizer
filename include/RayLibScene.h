#pragma once
#include "raylib.h"

class RaylibScene {
private:
    int m_sceneOriginX;
    int m_sceneOriginY;
    int m_sceneWidth;
    int m_sceneHeight;
    Camera m_camera;
    RenderTexture2D m_renderTarget;

public:
    RaylibScene(int originX, int originY, int width, int height);
    ~RaylibScene();
    void Init();
    void Update();
    void Draw();
};