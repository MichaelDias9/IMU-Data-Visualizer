#pragma once
#include "Config.h"
#include "ThreadSafeRingBuffer.h"

void runApp(GyroBuffer &gyroBuffer, AccelBuffer &accelBuffer, 
            MagBuffer &magBuffer, GyroBuffer& gyroTimeBuffer,
            AccelBuffer& accelTimeBuffer, MagBuffer& magTimeBuffer);