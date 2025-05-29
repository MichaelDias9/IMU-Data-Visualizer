#include "RunApp.h"
#include "Config.h"
#include "ImPlotPanel.h"
#include "ThreadSafeRingBuffer.h"

#include "rlImGui.h"
#include "imgui.h"
#include "implot.h"


void runApp(GyroBuffer &gyroBuffer, AccelBuffer& accelBuffer, MagBuffer& magBuffer, 
            std::array<float, gyroBufferSize>& gyroTimeBuffer, 
            std::array<float, accelBufferSize>& accelTimeBuffer, 
            std::array<float, magBufferSize>& magTimeBuffer) 
{ 
  // Initialize window
  InitWindow(screenWidth, screenHeight, "IMU + Attitude Estimation");
  SetTargetFPS(targetFrameRate);
  
  // Initialize ImGui and Plots 
  rlImGuiSetup(true);
  ImPlot::CreateContext();

  // Enable 32 bit vertex indices for more than 64K vertices
  ImGuiIO& io = ImGui::GetIO();
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  ImPlotPanel plotPanel(0, 0, screenWidth, screenHeight, 
                        gyroBuffer, accelBuffer, magBuffer, 
                        gyroTimeBuffer, accelTimeBuffer, magTimeBuffer
                        );


  // Run Main Loop
  while (!WindowShouldClose()) {
    // Draw frame
    BeginDrawing();
    ClearBackground(RAYWHITE);
          
    rlImGuiBegin();
    plotPanel.Draw();
    rlImGuiEnd();
    
    EndDrawing();
  }
  // Exit Gracefully
  ImPlot::DestroyContext();
  rlImGuiShutdown();
  CloseWindow();
}