#pragma once
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class RobotJoint {
private:
    float current_angle;
    float min_angle, max_angle;
    float max_speed; // deg/s
    glm::mat4 local_transform;
    glm::mat4 world_transform;

public:
    RobotJoint(float min_angle, float max_angle, float max_speed = 0.0f);
    void setAngle(float angle);
    void updateTransform(const glm::mat4& parent_transform);
    bool isAngleValid(float angle) const;
    glm::mat4 getTransform() const;
    float getCurrentAngle() const;
    float getMinAngle() const;
    float getMaxAngle() const;
};

class RobotArm {
private:
    std::array<RobotJoint, 6> joints;
    // Joint-specific parameters (from specifications)
    static constexpr float J1_MIN = -240.0f, J1_MAX = 240.0f, J1_SPEED = 225.0f;
    static constexpr float J2_MIN = -120.0f, J2_MAX = 120.0f, J2_SPEED = 150.0f;
    static constexpr float J3_MIN = 0.0f, J3_MAX = 160.0f, J3_SPEED = 275.0f;
    static constexpr float J4_MIN = -200.0f, J4_MAX = 200.0f, J4_SPEED = 412.0f;
    static constexpr float J5_MIN = -120.0f, J5_MAX = 120.0f, J5_SPEED = 450.0f;
    static constexpr float J6_MIN = -360.0f, J6_MAX = 360.0f, J6_SPEED = 720.0f;

public:
    RobotArm();
    void setJointAngles(const std::array<float, 6>& angles);
    void updateKinematics();
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    glm::mat4 getEndEffectorTransform() const;
    const std::array<RobotJoint, 6>& getJoints() const;
};