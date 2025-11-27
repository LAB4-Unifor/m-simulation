#pragma once
#include "raylib.h"
#include <string>
#include <array>

class RobotModel {
public:
    RobotModel();
    ~RobotModel();

    void Init(const std::string& path);
    void Unload();
    
    // Main draw function
    void Draw(const std::array<float, 6>& angles, float scale, bool wireframe);

    bool HasBones() const { return useModelMode; }

private:
    Model model;
    ModelAnimation* animations;
    int animCount;
    bool useModelMode;

    void UpdateBoneTransformations(const std::array<float, 6>& angles);
    void DrawProceduralGeometry(const std::array<float, 6>& angles, float scale, bool wireframe);
};