#include "robot_arm.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

RobotArm::RobotArm() {
    joints[0] = RobotJoint(0, -240.0f, 240.0f, 225.0f);
    joints[1] = RobotJoint(1, -120.0f, 120.0f, 150.0f);
    joints[2] = RobotJoint(2, 0.0f, 160.0f, 275.0f);
    joints[3] = RobotJoint(3, -200.0f, 200.0f, 412.0f);
    joints[4] = RobotJoint(4, -120.0f, 120.0f, 450.0f);
    joints[5] = RobotJoint(5, -360.0f, 360.0f, 720.0f);
    
    dh_parameters[0] = {0.0f, 0.1f, 0.0f, glm::radians(-90.0f)};
    dh_parameters[1] = {0.0f, 0.0f, 0.2f, 0.0f};
    dh_parameters[2] = {0.0f, 0.0f, 0.2f, 0.0f};
    dh_parameters[3] = {0.0f, 0.1f, 0.0f, glm::radians(-90.0f)};
    dh_parameters[4] = {0.0f, 0.0f, 0.0f, glm::radians(90.0f)};
    dh_parameters[5] = {0.0f, 0.05f, 0.0f, 0.0f};
    
    for (int i = 0; i < 6; i++) {
        isAnimating[i] = false;
        targetAngles[i] = 0.0f;
        animationSpeeds[i] = 0.0f;
    }
}

void RobotArm::initialize() {
    if (!loadModel("assets/Yuki.glb")) {
        std::cerr << "Failed to load robot model!" << std::endl;
    }
}

void RobotArm::update(float delta_time) {
    for (int i = 0; i < 6; i++) {
        if (isAnimating[i]) {
            float current = joints[i].getCurrentAngle();
            float target = targetAngles[i];
            float speed = animationSpeeds[i];
            
            float diff = target - current;
            float max_change = speed * delta_time;
            
            if (std::abs(diff) <= max_change) {
                joints[i].setAngle(target);
                isAnimating[i] = false;
            } else {
                joints[i].setAngle(current + std::copysign(max_change, diff));
            }
        }
    }
    
    updateDHParameters();
}

void RobotArm::render(const ShaderProgram& shader) {
    if (model.getMeshes().empty()) return;
    
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::scale(model_matrix, glm::vec3(0.01f));
    model.render(shader, model_matrix);
}

void RobotArm::setJointAngles(const std::array<float, 6>& angles, bool animate, float speed_factor) {
    for (int i = 0; i < 6; i++) {
        setJointAngle(i, angles[i], animate, speed_factor);
    }
}

void RobotArm::setJointAngle(int index, float angle, bool animate, float speed_factor) {
    if (index < 0 || index >= 6) return;
    
    if (animate) {
        targetAngles[index] = angle;
        animationSpeeds[index] = joints[index].getMaxSpeed() * speed_factor;
        isAnimating[index] = true;
    } else {
        joints[index].setAngle(angle);
        isAnimating[index] = false;
    }
}

void RobotArm::resetToHomePosition(bool animate) {
    std::array<float, 6> home_angles = {0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f};
    setJointAngles(home_angles, animate);
}

bool RobotArm::loadModel(const std::string& path) {
    return model.loadGLTF(path);
}

glm::mat4 RobotArm::getEndEffectorTransform() const {
    glm::mat4 transform = glm::mat4(1.0f);
    
    for (int i = 0; i < 6; i++) {
        const auto& dh = dh_parameters[i];
        transform = transform * calculateDHMatrix(dh.theta, dh.d, dh.a, dh.alpha);
    }
    
    return transform;
}

std::array<float, 6> RobotArm::getJointAngles() const {
    std::array<float, 6> angles;
    for (int i = 0; i < 6; i++) {
        angles[i] = joints[i].getCurrentAngle();
    }
    return angles;
}

void RobotArm::updateDHParameters() {
    for (int i = 0; i < 6; i++) {
        dh_parameters[i].theta = glm::radians(joints[i].getCurrentAngle());
    }
}

glm::mat4 RobotArm::calculateDHMatrix(float theta, float d, float a, float alpha) const {
    glm::mat4 matrix(1.0f);
    
    matrix[0][0] = cos(theta);
    matrix[0][1] = -sin(theta) * cos(alpha);
    matrix[0][2] = sin(theta) * sin(alpha);
    matrix[0][3] = a * cos(theta);
    
    matrix[1][0] = sin(theta);
    matrix[1][1] = cos(theta) * cos(alpha);
    matrix[1][2] = -cos(theta) * sin(alpha);
    matrix[1][3] = a * sin(theta);
    
    matrix[2][1] = sin(alpha);
    matrix[2][2] = cos(alpha);
    matrix[2][3] = d;
    
    return matrix;
}
