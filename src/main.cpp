#define _USE_MATH_DEFINES
#include <cmath>
#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <boost/asio.hpp>

#include "Config.h"
#include "WebSocketSession.h"
#include "RunApp.h"

int main() {
    GyroBuffer gyroBuffer;
    AccelBuffer accelBuffer;
    MagBuffer magBuffer;
    std::array<float, gyroBufferSize> gyroTimesBuffer;
    std::array<float, accelBufferSize> accelTimesBuffer;
    std::array<float, magBufferSize> magTimesBuffer;

    #pragma region Fill Buffers
    // Pre-fill gyro data buffer
    std::vector<float> gyroX(gyroBufferSize, 0.0f);
    std::vector<float> gyroY(gyroBufferSize, 0.0f);
    std::vector<float> gyroZ(gyroBufferSize, 0.0f);
    gyroBuffer.append(gyroX.data(), gyroY.data(), gyroZ.data(), gyroBufferSize);

    // Pre-fill accel data buffer
    std::vector<float> accelX(accelBufferSize, 0.0f);
    std::vector<float> accelY(accelBufferSize, 0.0f);
    std::vector<float> accelZ(accelBufferSize, 0.0f);
    accelBuffer.append(accelX.data(), accelY.data(), accelZ.data(), accelBufferSize);

    // Pre-fill mag data buffer
    std::vector<float> magX(magBufferSize, 0.0f);
    std::vector<float> magY(magBufferSize, 0.0f);
    std::vector<float> magZ(magBufferSize, 0.0f);
    magBuffer.append(magX.data(), magY.data(), magZ.data(), magBufferSize);

    // Pre-fill gyroscope time buffer
    float timeStep = 1.0f / gyroFreq;
    for (size_t i = 0; i < gyroBufferSize; ++i) {
        gyroTimesBuffer[i] = -bufferSeconds + i * timeStep;
    }

    // Pre-fill accelerometer time buffer
    timeStep = 1.0f / accelFreq;
    for (size_t i = 0; i < accelBufferSize; ++i) {
        accelTimesBuffer[i] = -bufferSeconds + i * timeStep;
    }

    // Pre-fill magnetometer time buffer
    timeStep = 1.0f / magFreq;
    for (size_t i = 0; i < magBufferSize; ++i) {
        magTimesBuffer[i] = -bufferSeconds + i * timeStep;
    }
    #pragma endregion

    // Optionally generate example data if testing without real data
    /*
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
    */
    
    // Run WebSocket server Thread
    boost::asio::io_context ioc;
    WebSocketSession server(ioc, 8000, gyroBuffer, accelBuffer, magBuffer);
    server.run();
    
    std::thread socketThread([&ioc]() { ioc.run(); });

    // Launch App Window
    runApp(gyroBuffer, accelBuffer, magBuffer, 
           gyroTimesBuffer, accelTimesBuffer, magTimesBuffer);

    // Cleanup
    ioc.stop();
    socketThread.join();
}