#include "robot_controller.h"
#include <algorithm>
#include <iostream>

RobotController::RobotController() : isDemoActive(false), demoTime(0.0f), isCollisionDetected(false) {
    currentAngles.fill(0.0f);
    targetAngles.fill(0.0f);
    
    // Posição Inicial "Ready" (Segura)
    SetAllTargets({0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f}); 
    currentAngles = targetAngles;
}

void RobotController::Update(float dt) {
    if (isDemoActive) UpdateDemo(dt);

    // Verifica Colisão (Segurança)
    CheckSelfCollision();

    // Velocidade de movimento
    float speed = isDemoActive ? 60.0f * dt : 120.0f * dt; // Graus por segundo

    for (int i = 0; i < 6; i++) {
        float diff = targetAngles[i] - currentAngles[i];
        
        // Se houver colisão, impede movimento que agrave (simplificado: para tudo)
        if (isCollisionDetected && std::abs(diff) > 0.01f) {
            // Em um sistema real, permitiríamos mover "para longe" da colisão
            // Aqui, apenas travamos visualmente para mostrar o erro
        }

        if (std::abs(diff) > 0.1f) {
            // Interpolação linear suave
            float step = speed * (diff > 0 ? 1.0f : -1.0f);
            if (std::abs(diff) < std::abs(step)) step = diff;
            currentAngles[i] += step;
        } else {
            currentAngles[i] = targetAngles[i];
        }
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

// --- IMPLEMENTAÇÃO DE CINEMÁTICA INVERSA (IK) ---
// Baseado na geometria descrita para o algoritmo FABRIK/Analítico
void RobotController::SetTargetPosition(Vector3 target) {
    // 1. Calcular J1 (Rotação da Base)
    // Se o robô estiver girando pelo lado errado, tente trocar x e z ou inverter sinais
    float theta1 = atan2(target.x, target.z) * RAD2DEG;
    SetJointTarget(0, theta1);

    // 2. Preparar geometria lateral (Plano 2D do braço)
    float r = sqrt(target.x*target.x + target.z*target.z);
    float y = target.y - L1; // Altura relativa ao ombro

    // 3. Lei dos Cossenos para J2 e J3
    float dist = sqrt(r*r + y*y);
    
    // Clamp para não esticar mais que o braço permite
    float totalLen = L2 + L3;
    if (dist > totalLen) dist = totalLen - 0.001f;

    // Ângulos internos do triângulo formado pelos elos
    float cos_beta = (L2*L2 + L3*L3 - dist*dist) / (2 * L2 * L3);
    cos_beta = std::clamp(cos_beta, -1.0f, 1.0f);
    float beta = acos(cos_beta); // Ângulo interno do cotovelo

    float cos_alpha = (L2*L2 + dist*dist - L3*L3) / (2 * L2 * dist);
    cos_alpha = std::clamp(cos_alpha, -1.0f, 1.0f);
    float alpha = acos(cos_alpha); // Ângulo interno do ombro

    // Inclinação da linha direta até o alvo
    float gamma = atan2(y, r);
   
    float theta2 = (PI/2 - (gamma + alpha)) * RAD2DEG; // Ombro
    float theta3 = (beta - PI) * RAD2DEG;              // Cotovelo (Inverti o sinal aqui) Ordem importa!

    // Se continuar invertido, tente descomentar a linha abaixo para inverter o J3:
    // theta3 = -theta3; 

    // Manda para o controlador com os limites de segurança
    SetJointTarget(1, theta2); 
    SetJointTarget(2, theta3); // Se o cotovelo dobrar pra "dentro" do braço, inverta o sinal.
    
    // Mantém a garra nivelada (Cinemática de Orientação Simples)
    // Tenta manter a ferramenta paralela ao chão compensando J2 e J3
    SetJointTarget(3, 0.0f);
    SetJointTarget(4, -(theta2 + theta3 + 90.0f)); 
}

// --- COLISÃO E FÍSICA ---
Vector3 RobotController::CalculateTipPosition() {
    // Forward Kinematics simplificado apenas para validar colisão
    // Retorna a posição aproximada do "Cotovelo" ou "Punho" para checar se bateu no chão/base
    // (Implementação completa exigiria matrizes 4x4, aqui usamos aproximação polar)
    float j1 = currentAngles[0] * DEG2RAD;
    float j2 = currentAngles[1] * DEG2RAD;
    float j3 = currentAngles[2] * DEG2RAD;

    // Posição do Cotovelo
    float x = sin(j1) * L2 * sin(j2);
    float y = L1 + L2 * cos(j2);
    float z = cos(j1) * L2 * sin(j2);
    
    return {x, y, z};
}

void RobotController::CheckSelfCollision() {
    isCollisionDetected = false;

    // Regra 1: Colisão com o chão
    // Se o ângulo J2 for muito baixo e J3 muito fechado, o braço bate na mesa
    if (currentAngles[1] > 100.0f) isCollisionDetected = true;
    if (currentAngles[1] < -100.0f) isCollisionDetected = true;

    // Regra 2: Auto-colisão (Cotovelo entrando na Base)
    // Simplificação baseada em limites angulares críticos do PDF
    if (currentAngles[2] > 155.0f) isCollisionDetected = true; // Limite físico do J3
}

void RobotController::UpdateDemo(float dt) {
    demoTime += dt;
    float t = fmod(demoTime, 6.0f);
    
    // Sequência de Demonstração que exercita os algoritmos
    if (t < 2.0f) SetTargetPosition({0.3f, 0.4f, 0.3f});      // Ponto Alto
    else if (t < 4.0f) SetTargetPosition({-0.3f, 0.2f, 0.3f}); // Ponto Baixo Esquerda
    else SetTargetPosition({0.0f, 0.5f, 0.4f});                // Ponto Central
}