#pragma once

#include <vector>

// Helper class to manage a circular buffer with running average and display window
class RingBuffer {
private:
    std::vector<float> data;
    size_t maxSize;       // Max size of entire buffer (e.g., 300)
    size_t displaySize;   // Number of points to display (e.g., 100)
    float runningSum;
    float runningAverage;

public:
    RingBuffer(size_t size, size_t display = 0) 
        : maxSize(size), displaySize(display == 0 ? size : display), 
          runningSum(0.0f), runningAverage(0.0f) {}

    void addValue(float value) {
        // Add new value to running sum
        runningSum += value;
        
        // Add value to buffer
        data.push_back(value);
        
        // If buffer is full, remove oldest value from sum
        if (data.size() > maxSize) {
            runningSum -= data.front();
            data.erase(data.begin());
        }
        
        // Update running average
        runningAverage = (data.size() > 0) ? runningSum / data.size() : 0.0f;
    }

    // Return all data in the buffer
    const std::vector<float>& getData() const {
        return data;
    }
    
    // Return only display window of data (most recent N points)
    std::vector<float> getDisplayData() const {
        if (data.size() <= displaySize) {
            return data;
        } else {
            return std::vector<float>(data.end() - displaySize, data.end());
        }
    }
    
    // Get start index for time alignment with display data
    size_t getDisplayOffset() const {
        return (data.size() > displaySize) ? (data.size() - displaySize) : 0;
    }
    
    float getAverage() const {
        return runningAverage;
    }
    
    size_t size() const {
        return data.size();
    }
};

// Helper struct to manage 3-axis sensor data
struct SensorDataBuffer {
    RingBuffer x;
    RingBuffer y;
    RingBuffer z;

    SensorDataBuffer(size_t bufferSize, size_t displaySize = 0) 
        : x(bufferSize, displaySize), 
          y(bufferSize, displaySize), 
          z(bufferSize, displaySize) {}
};