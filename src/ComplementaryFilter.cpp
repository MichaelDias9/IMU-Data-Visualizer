#include "ComplementaryFilter.h"
#include "raymath.h"

ComplementaryFilter::ComplementaryFilter(float alpha) 
    : alpha_(alpha) {
    attitude_.w = 1.0f;
    attitude_.x = attitude_.y = attitude_.z = 0.0f;
}

void ComplementaryFilter::update(float deltaTime, const Vector3f& acceleration, const Vector3f& gyro) {
    integrateGyro(gyro, deltaTime);
    correctWithAccelerometer(acceleration, deltaTime);
}

void ComplementaryFilter::integrateGyro(const Vector3f& gyro, float deltaTime) {
    Quaternion q = {attitude_.x, attitude_.y, attitude_.z, attitude_.w};
    Quaternion omega = {0, gyro.x, gyro.y, gyro.z};
    Quaternion qDot = QuaternionMultiply(q, omega);
    qDot = QuaternionScale(qDot, 0.5f * deltaTime);
    q = QuaternionAdd(q, qDot);
    q = QuaternionNormalize(q);

    attitude_.x = q.x;
    attitude_.y = q.y;
    attitude_.z = q.z;
    attitude_.w = q.w;
}

void ComplementaryFilter::correctWithAccelerometer(const Vector3f& acceleration, float deltaTime) {
    Vector3 accel = {acceleration.x, acceleration.y, acceleration.z};
    accel = Vector3Normalize(accel);

    Quaternion q = {attitude_.x, attitude_.y, attitude_.z, attitude_.w};
    Vector3 predicted = Vector3RotateByQuaternion({0, 0, 1}, q);
    Vector3 error = Vector3CrossProduct(predicted, accel);
    error = Vector3Scale(error, (1 - alpha_) * deltaTime);

    Quaternion correction = {error.x * 0.5f, error.y * 0.5f, error.z * 0.5f, 0};
    q = QuaternionAdd(q, correction);
    q = QuaternionNormalize(q);

    attitude_.x = q.x;
    attitude_.y = q.y;
    attitude_.z = q.z;
    attitude_.w = q.w;
}

Attitude ComplementaryFilter::getAttitude() const {
    return attitude_;
}