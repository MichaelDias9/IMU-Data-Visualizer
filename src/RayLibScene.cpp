#include "RayLibScene.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

// Constructor
RaylibScene::RaylibScene(int originX, int originY, int width, int height)
: m_sceneOriginX(originX), m_sceneOriginY(originY), m_sceneWidth(width), m_sceneHeight(height) {}

// Destructor
RaylibScene::~RaylibScene() {
    // Unload render texture when the scene is destroyed
    UnloadRenderTexture(m_renderTarget);
}

// Member function Init
void RaylibScene::Init() {
    // Initialize camera
    m_camera.position   = Vector3{  22.0f, 22.0f, 24.0f };
    m_camera.target     = Vector3{  0.0f,  0.0f,  0.0f };
    m_camera.up         = Vector3{  0.0f,  0.0f,  1.0f };
    m_camera.fovy       = 47.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;
    
    // Create render texture with the dimensions of the 3D scene area
    m_renderTarget = LoadRenderTexture(m_sceneWidth, m_sceneHeight);
}

// Member function Draw
void RaylibScene::Draw() {
    // First, render the 3D scene to the render texture
    BeginTextureMode(m_renderTarget);
        ClearBackground(WHITE);
        
        BeginMode3D(m_camera);
            // Draw Grid on X-Y plane
            rlPushMatrix();
                rlRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate grid to X-Y plane
                DrawGrid(25, 1.0f);
            rlPopMatrix();
            
            // Draw Coordinate Axes
            float axisRadius = 0.25f;    // Thickness of the axes
            int axisSlices = 16;         // More slices = smoother cylinder

            Vector3 origin = {0.0f, 0.0f, 0.0f};

            DrawCylinderEx(origin, Vector3{10.0f, 0.0f, 0.0f}, axisRadius, axisRadius, axisSlices, RED);    // X-axis
            DrawCylinderEx(origin, Vector3{0.0f, 10.0f, 0.0f}, axisRadius, axisRadius, axisSlices, GREEN);  // Y-axis
            DrawCylinderEx(origin, Vector3{0.0f, 0.0f, 10.0f}, axisRadius, axisRadius, axisSlices, BLUE);   // Z-axis

        EndMode3D();
    EndTextureMode();
    
    // Now draw the render texture to the screen at the desired position
    // Note: Render textures in raylib are y-flipped, so we need to adjust the source rectangle
    DrawTextureRec(
        m_renderTarget.texture,
        (Rectangle){ 0, 0, (float)m_sceneWidth, -(float)m_sceneHeight },
        (Vector2){ (float)m_sceneOriginX, (float)m_sceneOriginY },
        WHITE
    );
}

// Member function Update
void RaylibScene::Update() 
{

}