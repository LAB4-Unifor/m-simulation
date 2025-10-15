#include "camera.h"
#include <cmath>
#include <iostream>

Camera::Camera() 
    : position(0.0f, 0.0f, 5.0f),
      target(0.0f, 0.0f, 0.0f),
      up(0.0f, 1.0f, 0.0f),
      radius(5.0f),
      theta(0.0f),
      phi(glm::radians(45.0f)),
      fov(glm::radians(45.0f)),
      aspectRatio(16.0f / 9.0f),
      nearPlane(0.1f),
      farPlane(100.0f),
      autoRotate(false),
      autoRotateSpeed(1.0f) {
    updatePositionFromSpherical();
}

void Camera::update(float deltaTime) {
    if (autoRotate) {
        orbit(autoRotateSpeed * deltaTime, 0.0f);
    }
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::orbit(float deltaTheta, float deltaPhi) {
    theta += deltaTheta;
    phi += deltaPhi;
    
    const float minPhi = 0.01f;
    const float maxPhi = glm::pi<float>() - 0.01f;
    phi = glm::clamp(phi, minPhi, maxPhi);
    
    updatePositionFromSpherical();
}

void Camera::zoom(float delta) {
    radius -= delta;
    
    const float minRadius = 0.5f;
    const float maxRadius = 50.0f;
    radius = glm::clamp(radius, minRadius, maxRadius);
    
    updatePositionFromSpherical();
}

void Camera::pan(float deltaX, float deltaY) {
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(right, forward));
    
    float panSpeed = radius * 0.001f;
    
    glm::vec3 delta = right * deltaX * panSpeed + cameraUp * deltaY * panSpeed;
    target += delta;
    position += delta;
}

void Camera::setPosition(const glm::vec3& position) {
    this->position = position;
    
    glm::vec3 direction = target - position;
    radius = glm::length(direction);
    
    if (radius > 0.0f) {
        direction = direction / radius;
        theta = atan2f(direction.x, direction.z);
        phi = acosf(direction.y);
    }
}

void Camera::setTarget(const glm::vec3& target) {
    this->target = target;
    updatePositionFromSpherical();
}

void Camera::setUpVector(const glm::vec3& up) {
    this->up = up;
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
    this->fov = fov;
    this->aspectRatio = aspect;
    this->nearPlane = near;
    this->farPlane = far;
}

void Camera::setAutoRotate(bool enabled, float speed) {
    autoRotate = enabled;
    autoRotateSpeed = speed;
}

void Camera::updatePositionFromSpherical() {
    position.x = target.x + radius * sin(phi) * sin(theta);
    position.y = target.y + radius * cos(phi);
    position.z = target.z + radius * sin(phi) * cos(theta);
}
