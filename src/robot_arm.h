#pragma once
#include "raylib.h"
#include <vector>
#include <array>
#include <string>

class RobotArm {
public:
    RobotArm();
    ~RobotArm();

    void Init(const std::string& modelPath);
    void Update(float deltaTime);
    void Draw(float scale = 1.0f);
    void Unload();

    // Controles Manuais
    void SetJointAngle(int index, float angleDeg);
    float GetJointAngle(int index) const;

    // --- NOVAS FUNÇÕES DE SIMULAÇÃO ---
    void SetTargetAngles(const std::array<float, 6>& targets); // Move suavemente para todos os ângulos
    void StartDemoSequence();
    void StopDemoSequence();
    bool IsDemoActive() const { return isDemoActive; }
    
    // Presets
    void GoToHome();
    void GoToZero();
    void GoToReady();

    // Visualização Técnica
    void DrawWorkEnvelope(float scale);

private:
    Model model;
    ModelAnimation* animations;
    int animCount;
    
    // Estado
    std::array<float, 6> currentAngles;
    std::array<float, 6> targetAngles; // Para interpolação suave
    
    // Sistema de Demo
    bool isDemoActive;
    float demoTime;
    int demoStep;

    // Limites do Mitsubishi RV-2SDB
    const std::array<float, 6> lowerLimits = { -240.0f, -120.0f,   0.0f, -200.0f, -120.0f, -360.0f };
    const std::array<float, 6> upperLimits = {  240.0f,  120.0f, 160.0f,  200.0f,  120.0f,  360.0f };

    void UpdateModelNodes();
    void UpdateDemo(float dt);
};