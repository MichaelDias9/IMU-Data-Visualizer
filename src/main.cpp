#define _USE_MATH_DEFINES
#include <cmath>
#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>

#include "Config.h"
#include "RunApp.h"

int main() {
    
    GyroBuffer gyroBuffer;
    AccelBuffer accelBuffer;
    MagBuffer magBuffer;
    GyroBuffer gyroTimeBuffer;
    AccelBuffer accelTimeBuffer;
    MagBuffer magTimeBuffer;

    #pragma region Setup
    // Pre-fill gyro buffer
    std::vector<float> gyroX(gyroBufferSize, 0.0f);
    std::vector<float> gyroY(gyroBufferSize, 0.0f);
    std::vector<float> gyroZ(gyroBufferSize, 0.0f);
    gyroBuffer.append(gyroX.data(), gyroY.data(), gyroZ.data(), gyroBufferSize);

    // Pre-fill accel buffer
    std::vector<float> accelX(accelBufferSize, 0.0f);
    std::vector<float> accelY(accelBufferSize, 0.0f);
    std::vector<float> accelZ(accelBufferSize, 0.0f);
    accelBuffer.append(accelX.data(), accelY.data(), accelZ.data(), accelBufferSize);

    // Pre-fill mag buffer
    std::vector<float> magX(magBufferSize, 0.0f);
    std::vector<float> magY(magBufferSize, 0.0f);
    std::vector<float> magZ(magBufferSize, 0.0f);
    magBuffer.append(magX.data(), magY.data(), magZ.data(), magBufferSize);

    // Pre-fill gyroscope time buffer
    float timeStep = 1.0f / gyroFreq;
    std::vector<float> gyroTimeX(gyroBufferSize);
    std::vector<float> gyroTimeY(gyroBufferSize, 0.0f);
    std::vector<float> gyroTimeZ(gyroBufferSize, 0.0f);
    for (size_t i = 0; i < gyroBufferSize; ++i) {
        gyroTimeX[i] = -bufferSeconds + i * timeStep;
    }
    gyroTimeBuffer.append(gyroTimeX.data(), gyroTimeY.data(), gyroTimeZ.data(), gyroBufferSize);

    // Pre-fill accelerometer time buffer
    timeStep = 1.0f / accelFreq;
    std::vector<float> accelTimeX(accelBufferSize);
    std::vector<float> accelTimeY(accelBufferSize, 0.0f);
    std::vector<float> accelTimeZ(accelBufferSize, 0.0f);
    for (size_t i = 0; i < accelBufferSize; ++i) {
        accelTimeX[i] = -bufferSeconds + i * timeStep;
    }
    accelTimeBuffer.append(accelTimeX.data(), accelTimeY.data(), accelTimeZ.data(), accelBufferSize);

    // Pre-fill magnetometer time buffer
    timeStep = 1.0f / magFreq;
    std::vector<float> magTimeX(magBufferSize);
    std::vector<float> magTimeY(magBufferSize, 0.0f);
    std::vector<float> magTimeZ(magBufferSize, 0.0f);
    for (size_t i = 0; i < magBufferSize; ++i) {
        magTimeX[i] = -bufferSeconds + i * timeStep;
    }
    magTimeBuffer.append(magTimeX.data(), magTimeY.data(), magTimeZ.data(), magBufferSize);

    #pragma endregion

    #pragma region Threads

    // Gyro time thread
    std::thread gyroTimeThread([&gyroTimeBuffer]() {
        auto interval = std::chrono::milliseconds(1000 / gyroFreq);
        float t = 0.0f;
        while (true) {
            float x_val = t;
            float y_val = 0.0f;
            float z_val = 0.0f;
            gyroTimeBuffer.append(&x_val, &y_val, &z_val, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / gyroFreq;
        }
    });

    // Accel time thread
    std::thread accelTimeThread([&accelTimeBuffer]() {
        auto interval = std::chrono::milliseconds(1000 / accelFreq);
        float t = 0.0f;
        while (true) {
            float x_val = t;
            float y_val = 0.0f;
            float z_val = 0.0f;
            accelTimeBuffer.append(&x_val, &y_val, &z_val, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / accelFreq;
        }
    });

    // Gyro time thread
    std::thread magTimeThread([&magTimeBuffer]() {
        auto interval = std::chrono::milliseconds(1000 / magFreq);
        float t = 0.0f;
        while (true) {
            float x_val = t;
            float y_val = 0.0f;
            float z_val = 0.0f;
            magTimeBuffer.append(&x_val, &y_val, &z_val, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / magFreq;
        }
    });

    // Start data generation threads
    std::thread gyroThread([&gyroBuffer]() {
        const int frequency = gyroFreq;
        const float sineFrequency = 2.0f; // 2Hz sine wave
        auto interval = std::chrono::milliseconds(1000 / frequency);
        float t = 0.0f;
        while (true) {
            float xValue = std::sin(2 * M_PI * sineFrequency * t);
            float yValue = std::cos(2 * M_PI * sineFrequency * t);
            float zValue = std::tan(2 * M_PI * sineFrequency * t);

            gyroBuffer.append(&xValue, &yValue, &zValue, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / frequency;
        }
    });
    gyroThread.detach();

    std::thread accelThread([&accelBuffer]() {
        const int frequency = accelFreq;
        const float sineFrequency = 2.0f; // 2Hz sine wave
        auto interval = std::chrono::milliseconds(1000 / frequency);
        float t = 0.0f;
        while (true) {
            float xValue = std::sin(2 * M_PI * sineFrequency * t);
            float yValue = std::cos(2 * M_PI * sineFrequency * t);
            float zValue = std::tan(2 * M_PI * sineFrequency * t);

            accelBuffer.append(&xValue, &yValue, &zValue, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / frequency;
        }
    });
    accelThread.detach();

    std::thread magThread([&magBuffer]() {
        const int frequency = magFreq;
        const float sineFrequency = 2.0f; // 2Hz sine wave
        auto interval = std::chrono::milliseconds(1000 / frequency);
        float t = 0.0f;
        while (true) {
            float xValue = std::sin(2 * M_PI * sineFrequency * t);
            float yValue = std::cos(2 * M_PI * sineFrequency * t);
            float zValue = std::tan(2 * M_PI * sineFrequency * t);

            magBuffer.append(&xValue, &yValue, &zValue, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / frequency;
        }
    });
    magThread.detach();

    #pragma endregion
    
    // Launch App Window
    runApp(gyroBuffer, accelBuffer, magBuffer, gyroTimeBuffer, accelTimeBuffer, magTimeBuffer);
}