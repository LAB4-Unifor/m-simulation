#ifndef ROBOT_ARM_H
#define ROBOT_ARM_H

#include <array>
#include <glm/glm.hpp>
#include "robot_joint.h"
#include "model_loader.h"

class ShaderProgram;

class RobotArm {
public:
    RobotArm();
    
    void initialize();
    void update(float delta_time);
    void render(const ShaderProgram& shader);
    
    void setJointAngles(const std::array<float, 6>& angles, bool animate = false, float speed_factor = 1.0f);
    void setJointAngle(int index, float angle, bool animate = false, float speed_factor = 1.0f);
    void resetToHomePosition(bool animate = false);
    
    const std::array<RobotJoint, 6>& getJoints() const { return joints; }
    glm::mat4 getEndEffectorTransform() const;
    std::array<float, 6> getJointAngles() const;
    
    bool loadModel(const std::string& path);
    const ModelLoader& getModel() const { return model; }
    
private:
    std::array<RobotJoint, 6> joints;
    ModelLoader model;
    
    struct DHParameters {
        float theta;
        float d;
        float a;
        float alpha;
    };
    
    std::array<DHParameters, 6> dh_parameters;
    std::array<bool, 6> isAnimating;
    std::array<float, 6> targetAngles;
    std::array<float, 6> animationSpeeds;
    
    void updateDHParameters();
    glm::mat4 calculateDHMatrix(float theta, float d, float a, float alpha) const;
    
    // ADD THIS METHOD DECLARATION:
    void renderSimpleCube(const ShaderProgram& shader);
};

#endif