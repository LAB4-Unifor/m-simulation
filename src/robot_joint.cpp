#include "robot_joint.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

RobotJoint::RobotJoint(int index, float minAngle, float maxAngle, float maxSpeed)
    : index(index),
      current_angle(0.0f),
      target_angle(0.0f),
      min_angle(minAngle),
      max_angle(maxAngle),
      max_speed(maxSpeed),
      is_animating(false),
      animation_speed(0.0f) 
    {
        
    updateLocalTransform();
}

void RobotJoint::setAngle(float angle, bool validate) {
    if (validate && !isAngleValid(angle)) {
        std::cerr << "Warning: Joint " << index << " angle " << angle 
                  << " is outside valid range [" << min_angle << ", " << max_angle << "]" << std::endl;
        angle = glm::clamp(angle, min_angle, max_angle);
    }
    
    current_angle = angle;
    updateLocalTransform();
}

void RobotJoint::updateTransform(const glm::mat4& parent_transform) {
    world_transform = parent_transform * local_transform;
}

bool RobotJoint::isAngleValid(float angle) const {
    return angle >= min_angle && angle <= max_angle;
}

void RobotJoint::setTargetAngle(float target, float speed_factor) {
    if (!isAngleValid(target)) {
        std::cerr << "Warning: Joint " << index << " target angle " << target 
                  << " is outside valid range [" << min_angle << ", " << max_angle << "]" << std::endl;
        target = glm::clamp(target, min_angle, max_angle);
    }
    
    target_angle = target;
    is_animating = true;
    animation_speed = max_speed * speed_factor;
}

void RobotJoint::updateAnimation(float delta_time) {
    if (!is_animating) return;
    
    float angle_diff = target_angle - current_angle;
    float max_change = animation_speed * delta_time;
    
    if (glm::abs(angle_diff) <= max_change) {
        current_angle = target_angle;
        is_animating = false;
    } else {
        current_angle += glm::sign(angle_diff) * max_change;
    }
    
    updateLocalTransform();
}

void RobotJoint::updateLocalTransform() {
    switch (index) {
        case 0:
            local_transform = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case 1:
            local_transform = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 2:
            local_transform = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 3:
            local_transform = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), glm::vec3(0.0f, 0.0f, 1.0f));
            break;
        case 4:
            local_transform = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 5:
            local_transform = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), glm::vec3(0.0f, 0.0f, 1.0f));
            break;
        default:
            local_transform = glm::mat4(1.0f);
            break;
    }
}
