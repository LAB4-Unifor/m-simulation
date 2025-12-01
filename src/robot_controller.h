#pragma once
#include <array>
#include <vector>
#include <cmath>
#include "raylib.h"
#include "raymath.h"

class RobotController {
public:
    RobotController();

    void Update(float deltaTime);

    // --- Controle Manual (Cinemática Direta) ---
    void SetJointTarget(int index, float angle);
    void SetAllTargets(const std::array<float, 6>& targets);
    
    // --- Cinemática Inversa (FABRIK - TCC Pág 22) ---
    // Calcula os ângulos para levar o efetuador até (x,y,z)
    void SetTargetPosition(Vector3 targetPos);
    
    // --- Getters ---
    float GetJointAngle(int index) const;
    float GetTargetAngle(int index) const;
    std::array<float, 6> GetAllAngles() const { return currentAngles; }
    
    // --- Features ---
    void StartDemo();
    void StopDemo();
    bool IsDemoActive() const { return isDemoActive; }
    bool IsInCollision() const { return isCollisionDetected; }
    
    // Helpers
    // Calcula onde a ponta do robô está agora (Forward Kinematics)
    Vector3 CalculateTipPosition(); 

    // Limites (Baseado no PDF Tabela 1 e Brochure)
    static constexpr float LIMITS[6][2] = {
        {-240, 240}, // J1 Base
        {-120, 120}, // J2 Ombro
        {0, 160},    // J3 Cotovelo (Restrito para não colidir com chão)
        {-200, 200}, // J4
        {-120, 120}, // J5
        {-360, 360}  // J6
    };

    // Comprimentos dos elos (Metros - Ajustado para Escala Visual)
    // L1: Altura Base, L2: Braço, L3: Antebraço
    const float L1 = 0.35f; 
    const float L2 = 0.25f;
    const float L3 = 0.25f;

private:
    std::array<float, 6> currentAngles;
    std::array<float, 6> targetAngles;
    
    bool isDemoActive;
    float demoTime;
    bool isCollisionDetected;

    void UpdateDemo(float dt);
    void CheckSelfCollision();
    
};