#pragma once

#include "SharedData.h"
#include <chrono>

class ComplementaryFilter {
public:
    ComplementaryFilter(float alpha);
    void update(float deltaTime, const Vector3f& acceleration, const Vector3f& gyro);
    Attitude getAttitude() const;

private:
    float alpha_;
    Attitude attitude_;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate_;

    void integrateGyro(const Vector3f& gyro, float deltaTime);
    void correctWithAccelerometer(const Vector3f& acceleration, float deltaTime);
};