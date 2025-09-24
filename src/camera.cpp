#include "camera.h"
#include <cmath>
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, 
               float fov, float aspect_ratio, float near_plane, float far_plane)
    : position(position),
      target(target),
      up(up),
      radius(glm::length(position - target)),
      theta(0.0f),
      phi(glm::acos((position - target).y / radius)),
      fov(fov),
      aspect_ratio(aspect_ratio),
      near_plane(near_plane),
      far_plane(far_plane),
      auto_rotate_enabled(false),
      auto_rotate_speed(1.0f),
      min_radius(1.0f),
      max_radius(10.0f),
      min_phi(0.1f),
      max_phi(glm::pi<float>() - 0.1f) {
    
    // Ensure phi is within valid range
    phi = std::max(min_phi, std::min(max_phi, phi));
}

void Camera::orbit(float delta_theta, float delta_phi) {
    theta += delta_theta;
    phi += delta_phi;
    
    // Clamp phi to prevent gimbal lock
    phi = std::max(min_phi, std::min(max_phi, phi));
}

void Camera::zoom(float delta) {
    radius -= delta;
    radius = std::max(min_radius, std::min(max_radius, radius));
}

void Camera::pan(float delta_x, float delta_y) {
    // Convert to world space
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 up_world = glm::normalize(glm::cross(right, forward));
    
    // Calculate pan movement in world space
    glm::vec3 pan_vector = right * delta_x + up_world * delta_y;
    
    // Update target and position
    target += pan_vector;
    position += pan_vector;
}

void Camera::setAutoRotate(bool enabled, float speed) {
    auto_rotate_enabled = enabled;
    auto_rotate_speed = speed;
}

void Camera::update(float deltaTime) {
    if (auto_rotate_enabled) {
        theta += auto_rotate_speed * deltaTime;
    }
    
    // Update position based on spherical coordinates
    position.x = target.x + radius * glm::cos(phi) * glm::cos(theta);
    position.y = target.y + radius * glm::sin(phi);
    position.z = target.z + radius * glm::cos(phi) * glm::sin(theta);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
}