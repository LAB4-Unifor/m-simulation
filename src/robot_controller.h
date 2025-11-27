#pragma once
#include <array>
#include "raylib.h"

class RobotController {
public:
    RobotController();

    void Update(float deltaTime);

    // Setters
    void SetJointTarget(int index, float angle);
    void SetAllTargets(const std::array<float, 6>& targets);
    
    // Getters
    float GetJointAngle(int index) const;
    float GetTargetAngle(int index) const;
    std::array<float, 6> GetAllAngles() const { return currentAngles; }
    
    // Features
    void StartDemo();
    void StopDemo();
    bool IsDemoActive() const { return isDemoActive; }
    
    // Helpers
    Vector3 CalculateForwardKinematics();

    // Limits (Public for GUI to read)
    static constexpr float LIMITS[6][2] = {
        {-240, 240}, {-120, 120}, {0, 160}, 
        {-200, 200}, {-120, 120}, {-360, 360}
    };

private:
    std::array<float, 6> currentAngles;
    std::array<float, 6> targetAngles;
    
    bool isDemoActive;
    float demoTime;

    void UpdateDemo(float dt);
};