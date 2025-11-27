#include "robot_arm.h"
#include "raymath.h"
#include "rlgl.h"      // Necessário para manipulação de matrizes (rlPushMatrix, etc)
#include <iostream>
#include <algorithm>
#include <vector>

RobotArm::RobotArm() : animCount(0), animations(nullptr) {
    currentAngles.fill(0.0f);
    model = { 0 };
}

RobotArm::~RobotArm() {
    Unload();
}

void RobotArm::Init(const std::string& modelPath) {
    // Tenta carregar o modelo, mas não dependemos dele para a simulação funcionar agora
    if (FileExists(modelPath.c_str())) {
        model = LoadModel(modelPath.c_str());
        // Verificação de segurança para evitar crash se o arquivo for inválido
        if (model.meshCount > 0) {
            animations = LoadModelAnimations(modelPath.c_str(), &animCount);
            if (animCount > 0) TraceLog(LOG_INFO, "ROBOT: Esqueleto encontrado.");
            else TraceLog(LOG_WARNING, "ROBOT: Modelo sem ossos. Usando modo geometrico.");
        }
    } else {
        TraceLog(LOG_WARNING, "ROBOT: Arquivo nao encontrado. Usando modo geometrico.");
    }
}

void RobotArm::Unload() {
    if (animations) {
        UnloadModelAnimations(animations, animCount);
        animations = nullptr;
        animCount = 0;
    }
    if (model.meshCount > 0) {
        UnloadModel(model);
        model.meshCount = 0;
    }
}

void RobotArm::SetJointAngle(int index, float angle) {
    if (index < 0 || index >= 6) return;
    float clampedAngle = std::clamp(angle, lowerLimits[index], upperLimits[index]);
    currentAngles[index] = clampedAngle;
}

float RobotArm::GetJointAngle(int index) const {
    if (index < 0 || index >= 6) return 0.0f;
    return currentAngles[index];
}

void RobotArm::Update(float deltaTime) {
    // Se tivéssemos ossos reais, atualizaríamos aqui.
}

// Função auxiliar para desenhar eixos (debug)
void DrawAxis(float size) {
    DrawLine3D({0,0,0}, {size,0,0}, RED);   // X
    DrawLine3D({0,0,0}, {0,size,0}, GREEN); // Y
    DrawLine3D({0,0,0}, {0,0,size}, BLUE);  // Z
}

void RobotArm::Draw(float scale) {
    // Se tivermos um modelo COM ossos carregado, usamos ele.
    if (animCount > 0 && model.meshCount > 0) {
        // Lógica de desenho do modelo 3D (futuro)
        DrawModel(model, {0,0,0}, scale, WHITE);
    } else {
        // === MODO PROCEDURAL (ROBÔ GEOMÉTRICO) ===
        // Construído com primitivas do Raylib e matrizes do rlgl
        
        rlPushMatrix(); // Salva a matriz do mundo
        
        // Aplica a escala global
        rlScalef(scale, scale, scale);

        // --- BASE (Fixa) ---
        // Cilindro vertical: Posição, raio topo, raio base, altura, slices, cor
        DrawCylinder({0,0,0}, 0.6f, 0.6f, 0.5f, 16, DARKGRAY); 
        DrawAxis(1.5f);
        
        // --- JOINT 1 (Cintura - Gira em Y) ---
        rlTranslatef(0.0f, 0.5f, 0.0f); // Sobe
        rlRotatef(currentAngles[0], 0, 1, 0); // Gira
        
        DrawCube({0, 0.5f, 0}, 0.5f, 1.0f, 0.5f, RED); // Ombro visual
        
        // --- JOINT 2 (Ombro - Gira em Z) ---
        rlTranslatef(0.0f, 1.0f, 0.0f); 
        rlRotatef(currentAngles[1], 0, 0, 1); 
        
        // Braço Superior
        DrawCube({0, 1.0f, 0}, 0.4f, 2.0f, 0.4f, ORANGE); 
        
        // --- JOINT 3 (Cotovelo) ---
        rlTranslatef(0.0f, 2.0f, 0.0f); 
        rlRotatef(currentAngles[2], 0, 0, 1); 
        
        // Antebraço (horizontal)
        DrawCube({0.5f, 0.0f, 0}, 1.0f, 0.3f, 0.3f, YELLOW); 
        
        // --- JOINT 4 (Roll do Antebraço) ---
        rlTranslatef(1.0f, 0.0f, 0.0f); 
        rlRotatef(currentAngles[3], 1, 0, 0); 
        
        // Desenha um cilindro deitado para representar o pulso girando
        // DrawCylinderEx desenha de um ponto A para um ponto B
        DrawCylinderEx({0,0,0}, {0.5f, 0, 0}, 0.2f, 0.2f, 8, GREEN);
        
        // --- JOINT 5 (Pitch do Pulso) ---
        rlTranslatef(0.5f, 0.0f, 0.0f);
        rlRotatef(currentAngles[4], 0, 0, 1); 
        
        DrawCube({0,0,0}, 0.3f, 0.3f, 0.3f, BLUE); 
        
        // --- JOINT 6 (Yaw/Roll da Ferramenta) ---
        rlTranslatef(0.2f, 0.0f, 0.0f);
        rlRotatef(currentAngles[5], 1, 0, 0); 
        
        // End Effector (Cone)
        DrawCylinderEx({0,0,0}, {0.3f, 0, 0}, 0.1f, 0.0f, 16, PURPLE);
        
        rlPopMatrix(); // Restaura a matriz
    }
}

void RobotArm::UpdateModelNodes() {}