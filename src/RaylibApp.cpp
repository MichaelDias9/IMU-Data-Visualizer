#include "RaylibApp.h"
#include "SharedData.h"
#include "SensorDataBuffer.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "imgui.h"
#include "rlImGui.h"
#include "implot.h"
#include "implot_internal.h"

#include <cstdio>
#include <cmath>

int AccelerationXFormatter(double value, char* buff, int size, void* user_data) {
  double int_part;
  double frac_part = std::modf(value, &int_part);
  const double epsilon = 1e-8;
  if (std::abs(frac_part) < epsilon) {
      return snprintf(buff, size, "%.0f", value);
  } else {
      buff[0] = '\0'; // Empty string for decimal numbers
      return 0;
  }
}

void runRaylibApp()
{ 
  #pragma region INTIALIZE
  // Intialize Window
  const int screenWidth = 1680;
  const int screenHeight = 960;
  InitWindow(screenWidth, screenHeight, "Drone Attitude Estimation");
  SetTargetFPS(60);
  
  // Initialize rlImGui with dark theme and ImPlot
  rlImGuiSetup(true);
  ImPlot::CreateContext();
  // Initialize data buffers for plotting
  // Store 300 points, but only display the most recent 100
  const size_t totalBufferSize = 300;
  const size_t displayWindowSize = 100;
  
  SensorDataBuffer accelData(totalBufferSize, displayWindowSize);
  SensorDataBuffer angularVelData(totalBufferSize, displayWindowSize);
  SensorDataBuffer magFieldData(totalBufferSize, displayWindowSize);
  RingBuffer timeBuffer(totalBufferSize, displayWindowSize);
  float currentTime = 0.0f;
  // Initialize Raylib Camera
  Camera camera = { 0 };
  camera.position   = Vector3{  22.0f, 22.0f, 24.0f };
  camera.target     = Vector3{  0.0f,  0.0f,  0.0f };
  camera.up         = Vector3{  0.0f,  0.0f,  1.0f };
  camera.fovy       = 47.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  // Load Drone Model
  Model droneModel = LoadModel("resources/tinker.obj");
  float modelScale = 0.08f;
  
  #pragma endregion

  // --- MAIN LOOP ---
  while (!WindowShouldClose())
  {
    #pragma region UpdateLogic
    static float vertical_zoom = 1.0f;
    static float horizontal_zoom = 1.0f;
    static float min_zoom = 0.3f;
    static float max_zoom = 3.0f;
    
    // Get sensor data from SharedData
    Vector3f accel = SharedData::Instance().getAcceleration();
    Vector3f angularVel = SharedData::Instance().getRotationRate();
    Vector3f magneticField = SharedData::Instance().getMagneticField();

    // Update all buffers
    currentTime += GetFrameTime();
    timeBuffer.addValue(currentTime);
    accelData.x.addValue(accel.x);
    accelData.y.addValue(accel.y);
    accelData.z.addValue(accel.z);
    angularVelData.x.addValue(angularVel.x);
    angularVelData.y.addValue(angularVel.y);
    angularVelData.z.addValue(angularVel.z);
    magFieldData.x.addValue(magneticField.x);
    magFieldData.y.addValue(magneticField.y);
    magFieldData.z.addValue(magneticField.z);

    // Get the updated attitude as a rotation matrix
    Attitude att = SharedData::Instance().getAttitude();
    Quaternion Quat = {att.x, att.y, att.z, att.w};
    Matrix rotMat = QuaternionToMatrix(Quat);

    // Update vectors of data points to be displayed
    std::vector<float> displayTime = timeBuffer.getDisplayData();
    std::vector<float> accelX = accelData.x.getDisplayData();
    std::vector<float> accelY = accelData.y.getDisplayData();
    std::vector<float> accelZ = accelData.z.getDisplayData();
    std::vector<float> angVelX = angularVelData.x.getDisplayData();
    std::vector<float> angVelY = angularVelData.y.getDisplayData();
    std::vector<float> angVelZ = angularVelData.z.getDisplayData();
    
    #pragma endregion
    
    #pragma region Render       
    BeginDrawing();  
    ClearBackground(Color{ 65, 65, 200, 1 });

    #pragma region RaylibArea   // --- RAYLIB PANEL ---
    // Set viewport to bottom-right quadrant of screen
    rlViewport(2*screenWidth/3, 0, screenWidth/3, screenHeight/2);
    BeginMode3D(camera);
      
    // Draw Grid on X-Y plane
    rlPushMatrix();
      rlRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate grid to X-Y plane
      DrawGrid(25, 1.0f);
    rlPopMatrix();
    // Draw Coordinate Axes
    DrawLine3D(Vector3{0.0f, 0.0f, 0.0f}, Vector3{10.0f, 0.0f, 0.0f}, RED);    // X-axis
    DrawLine3D(Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 10.0f, 0.0f}, GREEN);  // Y-axis
    DrawLine3D(Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 10.0f}, BLUE);   // Z-axis
    // Draw Drone
    rlPushMatrix();
      // Apply the runtime rotation
      rlMultMatrixf(MatrixToFloat(rotMat));
      // Draw the model at its local (0,0,0)
      DrawModel(droneModel, Vector3Zero(), modelScale, WHITE);
    rlPopMatrix();
    EndMode3D();
    
    // Reset viewport for 2D elements
    rlViewport(0, 0, screenWidth, screenHeight);
    
    #pragma endregion RaylibArea
    rlImGuiBegin();
    #pragma region ImPlotArea   // --- IMPLOT PANEL ---
    // Render on left side of window
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(screenWidth*2/3, screenHeight), ImGuiCond_Always);
    
    ImGui::Begin("Sensor Data", nullptr, 
      ImGuiWindowFlags_NoMove | 
      ImGuiWindowFlags_NoResize | 
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_AlwaysVerticalScrollbar
    );
      // Handle Ctrl+Mouse Wheel for zoom
      if (ImGui::IsWindowHovered() && ImGui::GetIO().KeyCtrl) {
        vertical_zoom += ImGui::GetIO().MouseWheel * 0.1f;
        vertical_zoom = ImClamp(vertical_zoom, min_zoom, max_zoom);
      }

      // Handle Shift+Mouse Wheel for horizontal zoom
      if (ImGui::IsWindowHovered() && ImGui::GetIO().KeyShift) {
        horizontal_zoom += ImGui::GetIO().MouseWheel * 0.1f;
        horizontal_zoom = ImClamp(horizontal_zoom, min_zoom, max_zoom);
      }

      ImGui::Text("Real-time Sensor Data (Showing most recent %d of %d points):", 
                  displayWindowSize, totalBufferSize);

      // Zoom level info 
      ImGui::Text("Panel Zoom: %.1fx (Ctrl+Wheel)\nTime Axis Zoom %.1fx (Shift+Wheel)", 
            vertical_zoom, horizontal_zoom);
  
      // Calculate available plot height with zoom
      const float content_height = ImGui::GetContentRegionAvail().y;
      const float total_plots_height = content_height * vertical_zoom;
      const float plot_height = total_plots_height / 3.0f;

      // ACCELERATION PLOT 
      if (ImPlot::BeginPlot("Acceleration", ImVec2(-1, plot_height))) {
        ImPlot::SetupAxes("Time (s)", "m/s²");
        ImPlot::SetupAxisFormat(ImAxis_X1, AccelerationXFormatter);
        if (displayTime.size() > 1) {
          // Calculate zoomed x-axis range anchored to the right edge
          float time_latest = displayTime.back(); // Most recent time point (right edge)
          float time_range = displayTime.back() - displayTime.front(); // Current display range
          float zoomed_range = time_range / horizontal_zoom; // Apply zoom factor
          // Set x-axis limits with right edge anchored at the latest time point
          float x_min = time_latest - zoomed_range;
          float x_max = time_latest;
          // Set axis limits with zoomed range
          ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImGuiCond_Always);
          // Plot lines
          ImPlot::PlotLine("X", displayTime.data(), accelX.data(), displayTime.size());
          ImPlot::PlotLine("Y", displayTime.data(), accelY.data(), displayTime.size());
          ImPlot::PlotLine("Z", displayTime.data(), accelZ.data(), displayTime.size());
        }
        ImPlot::EndPlot();
      }

      ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y));
      
      // ANGULAR VELOCITY PLOT 
      if (ImPlot::BeginPlot("Angular Velocity", ImVec2(-1, plot_height))) {
        ImPlot::SetupAxes("Time (s)", "rad/s");
        ImPlot::SetupAxisFormat(ImAxis_X1, AccelerationXFormatter);
        if (displayTime.size() > 1) {
          // Calculate zoomed x-axis range anchored to the right edge
          float time_latest = displayTime.back(); // Most recent time point (right edge)
          float time_range = displayTime.back() - displayTime.front(); // Current display range
          float zoomed_range = time_range / horizontal_zoom; // Apply zoom factor
          // Set x-axis limits with right edge anchored at the latest time point
          float x_min = time_latest - zoomed_range;
          float x_max = time_latest;
          // Set axis limits with zoomed range
          ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImGuiCond_Always);
          // Plot Lines
          ImPlot::PlotLine("X", displayTime.data(), angVelX.data(), displayTime.size());
          ImPlot::PlotLine("Y", displayTime.data(), angVelY.data(), displayTime.size());
          ImPlot::PlotLine("Z", displayTime.data(), angVelZ.data(), displayTime.size());
        }
        ImPlot::EndPlot();
      }
      
    ImGui::End();
    #pragma endregion
    #pragma region ImGUIArea    // --- IMGUI PANEL --- 
    // --- SENSOR AVERAGES PANEL (TOP RIGHT) ---
    ImGui::SetNextWindowPos(ImVec2(screenWidth*2/3, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(screenWidth/3, screenHeight/2), ImGuiCond_Always);
    
    ImGui::Begin("Sensor Averages", nullptr, 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoCollapse
    );
      // Show buffer info
      ImGui::Text("Storage: %d points | Display: %d points", 
                totalBufferSize, displayWindowSize);
      ImGui::Text("Running Averages (Based on all %d samples)", totalBufferSize);
      ImGui::Separator();
      
      ImGui::Text("Acceleration Averages (m/s²):");
      ImGui::Indent(20.0f);
      ImGui::Text("X-axis: %.3f", accelData.x.getAverage());
      ImGui::Text("Y-axis: %.3f", accelData.y.getAverage());
      ImGui::Text("Z-axis: %.3f", accelData.z.getAverage());
      ImGui::Unindent(20.0f);
      
      ImGui::Separator();
      ImGui::Text("Angular Velocity Averages (rad/s):");
      ImGui::Indent(20.0f);
      ImGui::Text("X-axis: %.3f", angularVelData.x.getAverage());
      ImGui::Text("Y-axis: %.3f", angularVelData.y.getAverage());
      ImGui::Text("Z-axis: %.3f", angularVelData.z.getAverage());
      ImGui::Unindent(20.0f);
      
      ImGui::Separator();
      ImGui::Text("Magnetic Field Averages (µT):");
      ImGui::Indent(20.0f);
      ImGui::Text("X-axis: %.3f", magFieldData.x.getAverage());
      ImGui::Text("Y-axis: %.3f", magFieldData.y.getAverage());
      ImGui::Text("Z-axis: %.3f", magFieldData.z.getAverage());
      ImGui::Unindent(20.0f);
      
      // Calculate magnitude of vectors
      float accelMag = std::sqrt(
        accelData.x.getAverage() * accelData.x.getAverage() +
        accelData.y.getAverage() * accelData.y.getAverage() +
        accelData.z.getAverage() * accelData.z.getAverage()
      );
      
      ImGui::Separator();
      ImGui::Text("Vector Magnitudes:");
      ImGui::Indent(20.0f);
      ImGui::Text("Acceleration: %.3f m/s²", accelMag);
      ImGui::Unindent(20.0f);
    ImGui::End();
    #pragma endregion
    rlImGuiEnd(); 
    
    EndDrawing();
    #pragma endregion Render
  }

  // --- CLEAN UP ---
  ImPlot::DestroyContext();
  UnloadModel(droneModel);
  rlImGuiShutdown();

  // Close the window
  CloseWindow();
}