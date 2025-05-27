#pragma once
#include "Config.h"
#include "ThreadSafeRingBuffer.h"

void runApp(GyroBuffer &gyroBuffer, AccelBuffer &accelBuffer, MagBuffer &magBuffer, 
            std::array<float, gyroBufferSize>& gyroTimeBuffer, 
            std::array<float, accelBufferSize>& accelTimeBuffer, 
            std::array<float, magBufferSize>& magTimeBuffer
            );