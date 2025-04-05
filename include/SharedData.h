#pragma once

#include <mutex>

struct Attitude {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;
};

// For rotationRate, magneticField, totalAcceleration, we can store them as 3D floats:
struct Vector3f {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

class SharedData
{
public:
    // Singleton design pattern 
    static SharedData& Instance()
    {
        static SharedData instance;
        return instance;
    }

    // -------------- SETTERS --------------
    void setAttitude(float x, float y, float z, float w)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        attitude_.x  = x;
        attitude_.y = y;
        attitude_.z = z;
        attitude_.w = w;
    }

    void setRotationRate(float rx, float ry, float rz)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rotationRate_.x = rx;
        rotationRate_.y = ry;
        rotationRate_.z = rz;
    }

    void setMagneticField(float mx, float my, float mz)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        magneticField_.x = mx;
        magneticField_.y = my;
        magneticField_.z = mz;
    }

    void setAcceleration(float ax, float ay, float az)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        totalAcceleration_.x = ax;
        totalAcceleration_.y = ay;
        totalAcceleration_.z = az;
    }

    // -------------- GETTERS --------------
    Attitude getAttitude()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return attitude_;
    }

    Vector3f getRotationRate()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return rotationRate_;
    }

    Vector3f getMagneticField()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return magneticField_;
    }

    Vector3f getAcceleration()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalAcceleration_;
    }

private:
    // Private constructor for singleton
    SharedData() = default;
    ~SharedData() = default;

    // Non-copyable
    SharedData(const SharedData&)            = delete;
    SharedData& operator=(const SharedData&) = delete;

    std::mutex mutex_;

    // Data fields
    Attitude attitude_;
    Vector3f rotationRate_;
    Vector3f magneticField_;
    Vector3f totalAcceleration_;
};