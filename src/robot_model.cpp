#include "robot_model.h"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>

RobotModel::RobotModel() : animCount(0), animations(nullptr), useModelMode(false) {
    model = { 0 };
}

RobotModel::~RobotModel() {
    Unload();
}

void RobotModel::Init(const std::string& path) {
    if (FileExists(path.c_str())) {
        model = LoadModel(path.c_str());
        if (model.meshCount > 0) {
            animations = LoadModelAnimations(path.c_str(), &animCount);
            if (animCount > 0 && model.boneCount > 0) {
                useModelMode = true;
                TraceLog(LOG_INFO, "ROBOT: Skeleton found. Using 3D Model.");
            } else {
                TraceLog(LOG_WARNING, "ROBOT: No skeleton. Using Geometric Fallback.");
            }
        }
    } else {
        TraceLog(LOG_WARNING, "ROBOT: File not found. Using Geometric Fallback.");
    }
}

void RobotModel::Unload() {
    if (animations) UnloadModelAnimations(animations, animCount);
    if (model.meshCount > 0) UnloadModel(model);
}

void RobotModel::Draw(const std::array<float, 6>& angles, float scale, bool wireframe) {
    if (useModelMode) {
        UpdateBoneTransformations(angles);
        if (wireframe) DrawModelWires(model, {0,0,0}, scale, DARKGRAY);
        else DrawModel(model, {0,0,0}, scale, WHITE);
    } else {
        DrawProceduralGeometry(angles, scale, wireframe);
    }
}

void RobotModel::UpdateBoneTransformations(const std::array<float, 6>& angles) {
    if (!animations) return;
    ModelAnimation& anim = animations[0];
    int boneIndices[6] = { 1, 2, 3, 4, 5, 6 }; 

    for (int i = 0; i < 6; i++) {
        int boneIdx = boneIndices[i];
        if (boneIdx >= anim.boneCount) continue;
        Vector3 axis = { 0.0f, 1.0f, 0.0f }; 
        if (i == 1 || i == 2) axis = { 0.0f, 0.0f, 1.0f }; 
        Quaternion rotation = QuaternionFromAxisAngle(axis, angles[i] * DEG2RAD);
        anim.framePoses[0][boneIdx].rotation = rotation; 
    }
    UpdateModelAnimation(model, anim, 0);
}

void RobotModel::DrawProceduralGeometry(const std::array<float, 6>& angles, float scale, bool wireframe) {
    rlPushMatrix();
    rlScalef(scale, scale, scale);

    Color cBase = {50, 50, 50, 255};
    Color cGrey = {180, 180, 180, 255};
    Color cDark = {40, 40, 40, 255};

    // Base
    DrawCube({0, 0.2f, 0}, 0.8f, 0.4f, 0.8f, cBase);
    if(wireframe) DrawCubeWires({0, 0.2f, 0}, 0.8f, 0.4f, 0.8f, GREEN);

    // J1
    rlTranslatef(0.0f, 0.7f, 0.0f);
    rlRotatef(angles[0], 0, 1, 0);
    DrawCube({0, 0.4f, 0}, 0.5f, 0.8f, 0.6f, BLUE);
    if(wireframe) DrawCubeWires({0, 0.4f, 0}, 0.5f, 0.8f, 0.6f, GREEN);

    // J2
    rlTranslatef(0.0f, 0.8f, 0.0f);
    rlRotatef(angles[1], 0, 0, 1);
    DrawCylinderEx({0,0,-0.35f}, {0,0,0.35f}, 0.25f, 0.25f, 16, cDark);
    
    // Upper Arm
    rlTranslatef(0.0f, 0.0f, 0.2f); 
    DrawCube({0, 1.0f, 0}, 0.3f, 2.0f, 0.25f, YELLOW);
    if(wireframe) DrawCubeWires({0, 1.0f, 0}, 0.3f, 2.0f, 0.25f, GREEN);

    // J3
    rlTranslatef(0.0f, 2.0f, 0.0f);
    rlRotatef(angles[2], 0, 0, 1);
    DrawCylinderEx({0,0,-0.3f}, {0,0,0.1f}, 0.22f, 0.22f, 16, cDark);
    
    // Forearm
    DrawCube({0.5f, 0.0f, 0.0f}, 1.2f, 0.3f, 0.25f, GREEN);
    if(wireframe) DrawCubeWires({0.5f, 0.0f, 0.0f}, 1.2f, 0.3f, 0.25f, GREEN);

    // J4
    rlTranslatef(1.0f, 0.0f, 0.0f);
    rlRotatef(angles[3], 1, 0, 0);
    DrawCylinderEx({0,0,0}, {0.6f, 0, 0}, 0.18f, 0.15f, 12, cGrey);

    // J5
    rlTranslatef(0.6f, 0.0f, 0.0f);
    rlRotatef(angles[4], 0, 0, 1);
    DrawCube({0,0,0}, 0.25f, 0.3f, 0.25f, cDark);

    // J6
    rlTranslatef(0.15f, 0.0f, 0.0f);
    rlRotatef(angles[5], 1, 0, 0);
    DrawCylinderEx({0,0,0}, {0.05f, 0, 0}, 0.12f, 0.12f, 16, BLACK);

    rlPopMatrix();
}