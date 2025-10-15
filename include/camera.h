#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();
    
    void update(float deltaTime);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix() const;
    
    void orbit(float deltaTheta, float deltaPhi);
    void zoom(float delta);
    void pan(float deltaX, float deltaY);
    
    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);
    void setUpVector(const glm::vec3& up);
    void setPerspective(float fov, float aspect, float near, float far);
    
    void setAutoRotate(bool enabled, float speed = 1.0f);
    bool isAutoRotating() const { return autoRotate; }
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getTarget() const { return target; }
    glm::vec3 getUpVector() const { return up; }
    float getFOV() const { return fov; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    
    float radius;
    float theta;
    float phi;
    
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    
    bool autoRotate;
    float autoRotateSpeed;
    
    void updatePositionFromSpherical();
};

#endif
