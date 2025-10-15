#ifndef ROBOT_JOINT_H
#define ROBOT_JOINT_H

#include <glm/glm.hpp>

class RobotJoint {
public:

    RobotJoint() : index(-1), current_angle(0.0f), target_angle(0.0f),
                   min_angle(-180.0f), max_angle(180.0f), max_speed(90.0f),
                   local_transform(1.0f), world_transform(1.0f),
                   is_animating(false), animation_speed(0.0f) {
                    
                    updateLocalTransform();
                   }

    RobotJoint(int index, float minAngle, float maxAngle, float maxSpeed);
    
    void setAngle(float angle, bool validate = true);
    void updateTransform(const glm::mat4& parent_transform);
    bool isAngleValid(float angle) const;
    
    float getCurrentAngle() const { return current_angle; }
    float getMinAngle() const { return min_angle; }
    float getMaxAngle() const { return max_angle; }
    float getMaxSpeed() const { return max_speed; }
    glm::mat4 getTransform() const { return world_transform; }
    glm::mat4 getLocalTransform() const { return local_transform; }
    int getIndex() const { return index; }
    
    void setTargetAngle(float target, float speed_factor = 1.0f);
    void updateAnimation(float delta_time);
    bool isAnimating() const { return is_animating; }
    
private:
    int index;
    float current_angle;
    float target_angle;
    float min_angle;
    float max_angle;
    float max_speed;
    glm::mat4 local_transform;
    glm::mat4 world_transform;
    
    bool is_animating;
    float animation_speed;
    
    void updateLocalTransform();
};

#endif
