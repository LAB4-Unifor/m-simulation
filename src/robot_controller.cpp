#include "robot_controller.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

RobotController::RobotController() : isDemoActive(false), demoTime(0.0f) {
    currentAngles.fill(0.0f);
    // Start at HOME position
    SetAllTargets({0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f}); 
    currentAngles = targetAngles;
}

void RobotController::Update(float dt) {
    if (isDemoActive) UpdateDemo(dt);

    float speed = isDemoActive ? 4.0f * dt : 2.0f * dt;

    for (int i = 0; i < 6; i++) {
        float diff = targetAngles[i] - currentAngles[i];
        if (std::abs(diff) > 0.1f) currentAngles[i] += diff * speed;
        else currentAngles[i] = targetAngles[i];
    }
}

void RobotController::SetJointTarget(int index, float angle) {
    if (index < 0 || index >= 6) return;
    targetAngles[index] = std::clamp(angle, LIMITS[index][0], LIMITS[index][1]);
}

void RobotController::SetAllTargets(const std::array<float, 6>& targets) {
    for(int i=0; i<6; i++) SetJointTarget(i, targets[i]);
}

float RobotController::GetJointAngle(int index) const { return currentAngles[index]; }
float RobotController::GetTargetAngle(int index) const { return targetAngles[index]; }

void RobotController::StartDemo() { isDemoActive = true; demoTime = 0.0f; }
void RobotController::StopDemo() { isDemoActive = false; }

void RobotController::UpdateDemo(float dt) {
    demoTime += dt;
    float t = fmod(demoTime, 4.0f);
    if (t < 1.0f) SetAllTargets({45.0f, 30.0f, 45.0f, 0.0f, -45.0f, 0.0f});
    else if (t < 2.0f) SetAllTargets({45.0f, 45.0f, 80.0f, 0.0f, -45.0f, 0.0f});
    else if (t < 3.0f) SetAllTargets({-45.0f, 10.0f, 45.0f, 180.0f, -45.0f, 0.0f});
    else SetAllTargets({-45.0f, 40.0f, 70.0f, 0.0f, -45.0f, 0.0f});
}

Vector3 RobotController::CalculateForwardKinematics() {
    // Simple approx based on procedural geometry
    // Real Kinematics would require Denavit-Hartenberg parameters
    return {0, 0, 0}; // Placeholder
}