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
        
        // Apply calibration to get calibrated values
        calibratedRotationRate_.x = rx - gyroCalibrationOffset_.x;
        calibratedRotationRate_.y = ry - gyroCalibrationOffset_.y;
        calibratedRotationRate_.z = rz - gyroCalibrationOffset_.z;
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
        
        // Apply calibration to acceleration
        calibratedAcceleration_.x = ax - accelCalibrationOffset_.x;
        calibratedAcceleration_.y = ay - accelCalibrationOffset_.y;
        calibratedAcceleration_.z = az - accelCalibrationOffset_.z;
    }

    // -------------- CALIBRATION SETTERS --------------
    void setGyroCalibrationOffset(float offsetX, float offsetY, float offsetZ)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        gyroCalibrationOffset_.x = offsetX;
        gyroCalibrationOffset_.y = offsetY;
        gyroCalibrationOffset_.z = offsetZ;
        
        // Update calibrated values with new offsets
        calibratedRotationRate_.x = rotationRate_.x - gyroCalibrationOffset_.x;
        calibratedRotationRate_.y = rotationRate_.y - gyroCalibrationOffset_.y;
        calibratedRotationRate_.z = rotationRate_.z - gyroCalibrationOffset_.z;
    }

    void setAccelCalibrationOffset(float offsetX, float offsetY, float offsetZ)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        accelCalibrationOffset_.x = offsetX;
        accelCalibrationOffset_.y = offsetY;
        accelCalibrationOffset_.z = offsetZ;
        
        // Update calibrated acceleration with new offsets
        calibratedAcceleration_.x = totalAcceleration_.x - offsetX;
        calibratedAcceleration_.y = totalAcceleration_.y - offsetY;
        calibratedAcceleration_.z = totalAcceleration_.z - offsetZ;
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
    
    Vector3f getCalibratedRotationRate()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return calibratedRotationRate_;
    }

    Vector3f getGyroCalibrationOffset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return gyroCalibrationOffset_;
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

    Vector3f getCalibratedAcceleration()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return calibratedAcceleration_;
    }

    Vector3f getAccelCalibrationOffset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return accelCalibrationOffset_;
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
    
    // Calibration fields
    Vector3f gyroCalibrationOffset_ = {0.0f, 0.0f, 0.0f};
    Vector3f calibratedRotationRate_ = {0.0f, 0.0f, 0.0f};

    Vector3f accelCalibrationOffset_ = {0.0f, 0.0f, 0.0f};
    Vector3f calibratedAcceleration_ = {0.0f, 0.0f, 0.0f};
};