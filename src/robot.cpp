#include "robot.h"
#include <iostream>

RobotJoint::RobotJoint(float min_angle, float max_angle, float max_speed)
    : current_angle(0.0f),
      min_angle(min_angle),
      max_angle(max_angle),
      max_speed(max_speed) {
    
    // Initial position (identity matrix)
    local_transform = glm::mat4(1.0f);
    world_transform = glm::mat4(1.0f);
}

void RobotJoint::setAngle(float angle) {
    if (isAngleValid(angle)) {
        current_angle = angle;
        // Update local transform based on joint angle
        local_transform = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void RobotJoint::updateTransform(const glm::mat4& parent_transform) {
    world_transform = parent_transform * local_transform;
}

bool RobotJoint::isAngleValid(float angle) const {
    return angle >= min_angle && angle <= max_angle;
}

glm::mat4 RobotJoint::getTransform() const {
    return world_transform;
}

float RobotJoint::getCurrentAngle() const {
    return current_angle;
}

float RobotJoint::getMinAngle() const {
    return min_angle;
}

float RobotJoint::getMaxAngle() const {
    return max_angle;
}

RobotArm::RobotArm() {
    // Initialize joints with specifications
    joints[0] = RobotJoint(J1_MIN, J1_MAX, J1_SPEED);  // J1: Base rotation
    joints[1] = RobotJoint(J2_MIN, J2_MAX, J2_SPEED);  // J2: Shoulder
    joints[2] = RobotJoint(J3_MIN, J3_MAX, J3_SPEED);  // J3: Elbow
    joints[3] = RobotJoint(J4_MIN, J4_MAX, J4_SPEED);  // J4: Wrist roll
    joints[4] = RobotJoint(J5_MIN, J5_MAX, J5_SPEED);  // J5: Wrist pitch
    joints[5] = RobotJoint(J6_MIN, J6_MAX, J6_SPEED);  // J6: Wrist yaw
}

void RobotArm::setJointAngles(const std::array<float, 6>& angles) {
    for (int i = 0; i < 6; ++i) {
        joints[i].setAngle(angles[i]);
    }
}

void RobotArm::updateKinematics() {
    // Start with identity matrix for base
    glm::mat4 parent_transform = glm::mat4(1.0f);
    
    // Update each joint's transformation based on parent
    for (int i = 0; i < 6; ++i) {
        joints[i].updateTransform(parent_transform);
        parent_transform = joints[i].getTransform();
    }
}

void RobotArm::render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    // Start with identity matrix for base
    glm::mat4 parent_transform = model;
    
    // Render each joint in sequence
    for (int i = 0; i < 6; ++i) {
        // Apply joint transformation
        glm::mat4 joint_transform = joints[i].getTransform();
        
        // Render the joint (we'll use a simple cylinder for visualization)
        glm::mat4 model_matrix = parent_transform * joint_transform;
        
        // Update parent for next joint
        parent_transform = model_matrix;
    }
}

glm::mat4 RobotArm::getEndEffectorTransform() const {
    return joints[5].getTransform();
}

const std::array<RobotJoint, 6>& RobotArm::getJoints() const {
    return joints;
}