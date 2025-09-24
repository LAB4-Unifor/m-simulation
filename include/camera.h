#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float radius, theta, phi;  // Spherical coordinates
    float fov, aspect_ratio, near_plane, far_plane;
    bool auto_rotate_enabled;
    float auto_rotate_speed;
    float min_radius, max_radius;
    float min_phi, max_phi;

public:
    Camera(glm::vec3 position = glm::vec3(5.0f, 5.0f, 5.0f),
           glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float fov = 45.0f, 
           float aspect_ratio = 16.0f/9.0f,
           float near_plane = 0.1f, 
           float far_plane = 100.0f);

    void orbit(float delta_theta, float delta_phi);
    void zoom(float delta);
    void pan(float delta_x, float delta_y);
    void setAutoRotate(bool enabled, float speed = 1.0f);
    void update(float deltaTime);
    void setRadius(float radius);
    void setFov(float fov);
    void setAspectRatio(float aspect_ratio);
    void setNearFarPlanes(float near, float far);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    
    // Getters
    glm::vec3 getPosition() const { return position; }
    float getRadius() const { return radius; }
    float getFov() const { return fov; }
    bool getAutoRotateEnabled() const { return auto_rotate_enabled; }
};