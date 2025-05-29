#pragma once
#include "ThreadSafeRingBuffer.h"
#include <cstddef>

const int screenWidth = 1280;
const int screenHeight = 800;
const int targetFrameRate = 120;

constexpr int gyroFreq = 100;
constexpr int accelFreq = 200;
constexpr int magFreq = 200;
constexpr int bufferSeconds = 5;

constexpr std::size_t gyroBufferSize = gyroFreq * bufferSeconds;
constexpr std::size_t accelBufferSize = accelFreq * bufferSeconds;
constexpr std::size_t magBufferSize = magFreq * bufferSeconds;

using GyroBuffer = ThreadSafeRingBuffer<gyroBufferSize>;
using AccelBuffer = ThreadSafeRingBuffer<accelBufferSize>;
using MagBuffer = ThreadSafeRingBuffer<magBufferSize>;