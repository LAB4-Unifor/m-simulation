#include "robot_arm.h"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>
#include <algorithm>
#include <cmath>

RobotArm::RobotArm() : animCount(0), animations(nullptr), isDemoActive(false), demoTime(0.0f), demoStep(0) {
    currentAngles.fill(0.0f);
    targetAngles.fill(0.0f);
    model = { 0 };
}

RobotArm::~RobotArm() {
    Unload();
}

void RobotArm::Init(const std::string& modelPath) {
    if (FileExists(modelPath.c_str())) {
        model = LoadModel(modelPath.c_str());
        if (model.meshCount > 0) {
            animations = LoadModelAnimations(modelPath.c_str(), &animCount);
        }
    }
    // Define a posição inicial como Home
    GoToHome();
}

void RobotArm::Unload() {
    if (animations) UnloadModelAnimations(animations, animCount);
    if (model.meshCount > 0) UnloadModel(model);
}

void RobotArm::SetJointAngle(int index, float angle) {
    if (index < 0 || index >= 6) return;
    float clamped = std::clamp(angle, lowerLimits[index], upperLimits[index]);
    targetAngles[index] = clamped; // Agora definimos o ALVO, não o valor imediato
}

float RobotArm::GetJointAngle(int index) const {
    return currentAngles[index];
}

void RobotArm::SetTargetAngles(const std::array<float, 6>& targets) {
    for(int i=0; i<6; i++) {
        SetJointAngle(i, targets[i]);
    }
}

// --- PRESETS ---
void RobotArm::GoToHome() {
    // Posição de descanso padrão
    SetTargetAngles({0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f});
}

void RobotArm::GoToZero() {
    SetTargetAngles({0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
}

void RobotArm::GoToReady() {
    // Posição pronta para pegar algo na mesa
    SetTargetAngles({0.0f, 30.0f, 60.0f, 0.0f, -90.0f, 0.0f});
}

// --- DEMO SYSTEM ---
void RobotArm::StartDemoSequence() {
    isDemoActive = true;
    demoTime = 0.0f;
    demoStep = 0;
}

void RobotArm::StopDemoSequence() {
    isDemoActive = false;
}

void RobotArm::Update(float deltaTime) {
    // 1. Atualiza a lógica da Demo se estiver ativa
    if (isDemoActive) {
        UpdateDemo(deltaTime);
    }

    // 2. Interpolação Suave (Lerp)
    // Move os ângulos atuais em direção aos alvos
    float speed = 2.0f * deltaTime; // Velocidade de convergência
    
    // Se estiver em demo, movemos mais rápido
    if (isDemoActive) speed = 4.0f * deltaTime;

    for (int i = 0; i < 6; i++) {
        // Interpolação linear simples: Current + (Target - Current) * Factor
        float diff = targetAngles[i] - currentAngles[i];
        if (std::abs(diff) > 0.1f) {
            currentAngles[i] += diff * speed;
        } else {
            currentAngles[i] = targetAngles[i]; // Chegou no alvo
        }
    }
}

void RobotArm::UpdateDemo(float dt) {
    demoTime += dt;
    
    // Máquina de estados simples baseada em tempo
    // Ciclo de 4 segundos
    float cycleTime = 4.0f; 
    float t = fmod(demoTime, cycleTime);
    
    if (t < 1.0f) {
        // Passo 1: Girar base e descer
        SetTargetAngles({45.0f, 30.0f, 45.0f, 0.0f, -45.0f, 0.0f});
    } else if (t < 2.0f) {
        // Passo 2: Pegar (Simulado)
        SetTargetAngles({45.0f, 45.0f, 80.0f, 0.0f, -45.0f, 0.0f});
    } else if (t < 3.0f) {
        // Passo 3: Subir e girar para outro lado
        SetTargetAngles({-45.0f, 10.0f, 45.0f, 180.0f, -45.0f, 0.0f});
    } else {
        // Passo 4: Soltar
        SetTargetAngles({-45.0f, 40.0f, 70.0f, 0.0f, -45.0f, 0.0f});
    }
}

void RobotArm::DrawWorkEnvelope(float scale) {
    // Desenha uma esfera transparente representando o alcance máximo (Raio ~500-600mm)
    // Ajustado visualmente para a escala do modelo geométrico
    DrawSphereWires({0, 1.0f * scale, 0}, 3.0f * scale, 16, 16, {0, 255, 0, 50});
}

// --- DESENHO (Mantido do anterior, mas agora usa currentAngles suavizados) ---
void RobotArm::Draw(float scale) {
    // ... [MANTENHA O CÓDIGO DO ROBÔ GEOMÉTRICO QUE TE PASSEI ANTES AQUI] ...
    // Vou resumir para não ficar gigante, mas você deve copiar o corpo do Draw anterior.
    // Apenas certifique-se de usar 'currentAngles' que agora são suavizados.
    
    if (animCount > 0 && model.meshCount > 0) {
        DrawModel(model, {0,0,0}, scale, WHITE);
    } else {
        // MODO PROCEDURAL
        rlPushMatrix();
        rlScalef(scale, scale, scale);
        
        // Base
        DrawCylinder({0,0,0}, 0.6f, 0.6f, 0.5f, 16, DARKGRAY);
        
        // J1
        rlTranslatef(0.0f, 0.5f, 0.0f);
        rlRotatef(currentAngles[0], 0, 1, 0);
        DrawCube({0, 0.5f, 0}, 0.5f, 1.0f, 0.5f, RED);
        
        // J2
        rlTranslatef(0.0f, 1.0f, 0.0f);
        rlRotatef(currentAngles[1], 0, 0, 1);
        DrawCube({0, 1.0f, 0}, 0.4f, 2.0f, 0.4f, ORANGE);
        
        // J3
        rlTranslatef(0.0f, 2.0f, 0.0f);
        rlRotatef(currentAngles[2], 0, 0, 1);
        DrawCube({0.5f, 0.0f, 0}, 1.0f, 0.3f, 0.3f, YELLOW);
        
        // J4
        rlTranslatef(1.0f, 0.0f, 0.0f);
        rlRotatef(currentAngles[3], 1, 0, 0);
        DrawCylinderEx({0,0,0}, {0.5f, 0, 0}, 0.2f, 0.2f, 8, GREEN);
        
        // J5
        rlTranslatef(0.5f, 0.0f, 0.0f);
        rlRotatef(currentAngles[4], 0, 0, 1);
        DrawCube({0,0,0}, 0.3f, 0.3f, 0.3f, BLUE);
        
        // J6
        rlTranslatef(0.2f, 0.0f, 0.0f);
        rlRotatef(currentAngles[5], 1, 0, 0);
        DrawCylinderEx({0,0,0}, {0.3f, 0, 0}, 0.1f, 0.0f, 16, PURPLE);
        
        rlPopMatrix();
    }
}
