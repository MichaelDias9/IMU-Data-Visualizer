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

void prefillBuffers(GyroBuffer& gyroBuffer, AccelBuffer& accelBuffer, MagBuffer& magBuffer,
    std::array<float, gyroBufferSize>& gyroTimesBuffer,
    std::array<float, accelBufferSize>& accelTimesBuffer,
    std::array<float, magBufferSize>& magTimesBuffer);

void startExampleDataThreads(GyroBuffer& gyroBuffer, AccelBuffer& accelBuffer, MagBuffer& magBuffer);

int main() {
    GyroBuffer gyroBuffer;
    AccelBuffer accelBuffer;
    MagBuffer magBuffer;

    std::array<float, gyroBufferSize> gyroTimesBuffer;
    std::array<float, accelBufferSize> accelTimesBuffer;
    std::array<float, magBufferSize> magTimesBuffer;

    prefillBuffers(gyroBuffer, accelBuffer, magBuffer,
                         gyroTimesBuffer, accelTimesBuffer, magTimesBuffer);

    startExampleDataThreads(gyroBuffer, accelBuffer, magBuffer); // Use for example data 

    // Start WebSocket server
    boost::asio::io_context ioc;
    WebSocketSession server(ioc, 8000, gyroBuffer, accelBuffer, magBuffer);
    server.run();
    std::thread socketThread([&ioc]() { ioc.run(); });

    // Launch application UI
    runApp(gyroBuffer, accelBuffer, magBuffer,
           gyroTimesBuffer, accelTimesBuffer, magTimesBuffer);

    // Clean up
    ioc.stop();
    socketThread.join();
}

void prefillBuffers(GyroBuffer& gyroBuffer, AccelBuffer& accelBuffer, MagBuffer& magBuffer,
                    std::array<float, gyroBufferSize>& gyroTimesBuffer,
                    std::array<float, accelBufferSize>& accelTimesBuffer,
                    std::array<float, magBufferSize>& magTimesBuffer) {
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

    // Time buffers
    float timeStep;

    timeStep = 1.0f / gyroFreq;
    for (size_t i = 0; i < gyroBufferSize; ++i)
        gyroTimesBuffer[i] = -bufferSeconds + i * timeStep;

    timeStep = 1.0f / accelFreq;
    for (size_t i = 0; i < accelBufferSize; ++i)
        accelTimesBuffer[i] = -bufferSeconds + i * timeStep;

    timeStep = 1.0f / magFreq;
    for (size_t i = 0; i < magBufferSize; ++i)
        magTimesBuffer[i] = -bufferSeconds + i * timeStep;
}

void startExampleDataThreads(GyroBuffer& gyroBuffer, AccelBuffer& accelBuffer, MagBuffer& magBuffer) {
    std::thread([&gyroBuffer]() {
        const int sampleFrequency = gyroFreq;
        const float dataFrequency = 0.5f;
        auto interval = std::chrono::milliseconds(1000 / sampleFrequency);
        float t = 0.0f;
        while (true) {
            float x = std::sin(2 * M_PI * dataFrequency * t);
            float y = std::cos(2 * M_PI * dataFrequency * 0.5f * t);
            float z = std::sin(2 * M_PI * dataFrequency * (1.0f / 3.0f) * t);
            gyroBuffer.append(&x, &y, &z, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / sampleFrequency;
        }
    }).detach();

    std::thread([&accelBuffer]() {
        const int sampleFrequency = accelFreq;
        const float dataFrequency = 1.0f;
        auto interval = std::chrono::milliseconds(1000 / sampleFrequency);
        float t = 0.0f;
        while (true) {
            float x = std::sin(2 * M_PI * dataFrequency * t);
            float y = std::cos(2 * M_PI * dataFrequency * 0.5f * t);
            float z = std::sin(2 * M_PI * dataFrequency * (1.0f / 3.0f) * t);
            accelBuffer.append(&x, &y, &z, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / sampleFrequency;
        }
    }).detach();

    std::thread([&magBuffer]() {
        const int sampleFrequency = magFreq;
        const float dataFrequency = 2.0f;
        auto interval = std::chrono::milliseconds(1000 / sampleFrequency);
        float t = 0.0f;
        while (true) {
            float x = std::sin(2 * M_PI * dataFrequency * t);
            float y = std::cos(2 * M_PI * dataFrequency * 0.5f * t);
            float z = std::sin(2 * M_PI * dataFrequency * (1.0f / 3.0f) * t);
            magBuffer.append(&x, &y, &z, 1);
            std::this_thread::sleep_for(interval);
            t += 1.0f / sampleFrequency;
        }
    }).detach();
}