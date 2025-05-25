#include "RunApp.h"
#include "RayLibScene.h"
#include "ImPlotPanel.h"
#include "ImGuiPanel.h"
#include "Config.h"
#include "ThreadSafeRingBuffer.h"

#include "imgui.h"
#include "rlImGui.h"
#include "implot.h"

#include <cstdio>
#include <cmath>

void runApp(GyroBuffer &gyroBuffer, AccelBuffer &accelBuffer, 
            MagBuffer &magBuffer, GyroBuffer& gyroTimeBuffer,
            AccelBuffer& accelTimeBuffer, MagBuffer& magTimeBuffer) 
{ 
  // Initialize window
  InitWindow(screenWidth, screenHeight, "IMU + Attitude Estimation");
  SetTargetFPS(60);

  // Initialize Raylib Scene
  RaylibScene raylibScene(screenWidth/2, screenHeight/2, screenWidth/2, screenHeight/2);
  raylibScene.Init();
  
  // Initialize Plots 
  rlImGuiSetup(true);
  ImPlot::CreateContext();
  ImPlotPanel plotPanel(0, 0, screenWidth/2, screenHeight, 
                        gyroBuffer, accelBuffer, magBuffer, gyroTimeBuffer, accelTimeBuffer, magTimeBuffer);

  // Initialize GUI
  float exampleData = 0.0f;
  ImGuiPanel guiPanel(screenWidth/2, 0, screenWidth/2, screenHeight/2, &exampleData);
  
  // Run Main Loop
  while (!WindowShouldClose()) {
    // Update frame
    raylibScene.Update();

    // Draw frame
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    raylibScene.Draw();
      
    rlImGuiBegin();

    plotPanel.Draw();
    guiPanel.Draw();

    rlImGuiEnd();
    
    EndDrawing();
  }
  // Exit Gracefully
  ImPlot::DestroyContext();
  rlImGuiShutdown();
  CloseWindow();
}