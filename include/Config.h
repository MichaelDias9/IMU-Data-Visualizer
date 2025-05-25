#pragma once
#include "ThreadSafeRingBuffer.h"
#include <cstddef>

const int screenWidth = 1280;
const int screenHeight = 800;

constexpr int gyroFreq = 1000;
constexpr int accelFreq = 500;
constexpr int magFreq = 250;
constexpr int bufferSeconds = 10;

constexpr std::size_t gyroBufferSize = gyroFreq * bufferSeconds;
constexpr std::size_t accelBufferSize = accelFreq * bufferSeconds;
constexpr std::size_t magBufferSize = magFreq * bufferSeconds;


using GyroBuffer = ThreadSafeRingBuffer<gyroBufferSize>;
using AccelBuffer = ThreadSafeRingBuffer<accelBufferSize>;
using MagBuffer = ThreadSafeRingBuffer<magBufferSize>;