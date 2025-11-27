#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"
#include "robot_arm.h"
#include "robot_communication.h"
#include <string>

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Mitsubishi RV-2SDB Simulation");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 8.0f, 8.0f, 8.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    RobotArm robot;
    robot.Init("assets/Robot.glb");

    RobotCommunication comms;
    comms.Connect("127.0.0.1", 502);

    // Controle da GUI
    bool manualControl = true;
    float modelScale = 2.0f; // Ajustado para o modo procedural
    bool showEnvelope = false;

    DisableCursor(); 

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        if (IsKeyPressed(KEY_TAB)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }

        if (IsCursorHidden()) UpdateCamera(&camera, CAMERA_FREE);

        // Atualização Lógica
        robot.Update(dt);

        BeginDrawing();
            ClearBackground({ 30, 30, 30, 255 });

            BeginMode3D(camera);
                DrawGrid(20, 1.0f); 
                
                // Desenha o Robô
                robot.Draw(modelScale);
                
                // Desenha o Envelope de Trabalho (Transparente)
                if (showEnvelope) {
                    robot.DrawWorkEnvelope(modelScale);
                }
            EndMode3D();

            rlImGuiBegin();

            // Janela de Controle Principal
            ImGui::Begin("Mitsubishi Controller", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            
            ImGui::SeparatorText("Simulation Mode");
            
            // Botões de Presets
            if (ImGui::Button("HOME")) robot.GoToHome(); 
            ImGui::SameLine();
            if (ImGui::Button("ZERO")) robot.GoToZero();
            ImGui::SameLine();
            if (ImGui::Button("READY")) robot.GoToReady();

            ImGui::Spacing();

            // Botão de Demo
            if (robot.IsDemoActive()) {
                if (ImGui::Button("STOP DEMO", ImVec2(-1, 0))) {
                    robot.StopDemoSequence();
                    manualControl = true;
                }
            } else {
                if (ImGui::Button("RUN TEST DEMO", ImVec2(-1, 0))) {
                    robot.StartDemoSequence();
                    manualControl = false; // Demo assume o controle
                }
            }

            ImGui::SeparatorText("Visual Helpers");
            ImGui::Checkbox("Show Work Envelope", &showEnvelope);
            ImGui::SliderFloat("Scale", &modelScale, 0.5f, 5.0f);

            ImGui::SeparatorText("Joint Control (Manual)");
            
            // Desabilita sliders se a demo estiver rodando
            if (robot.IsDemoActive()) ImGui::BeginDisabled();

            const float limits[6][2] = {
                {-240, 240}, {-120, 120}, {0, 160}, 
                {-200, 200}, {-120, 120}, {-360, 360}
            };

            for (int i = 0; i < 6; i++) {
                // Pegamos o Target para o slider não pular enquanto o robô se move
                float angle = robot.GetJointAngle(i); 
                std::string label = "J" + std::to_string(i + 1);
                
                if (ImGui::SliderFloat(label.c_str(), &angle, limits[i][0], limits[i][1])) {
                    robot.SetJointAngle(i, angle);
                }
            }

            if (robot.IsDemoActive()) ImGui::EndDisabled();

            ImGui::End();
            
            // Overlay de Informação
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
            ImGui::TextColored(ImVec4(0,1,0,1), "FPS: %d", GetFPS());
            ImGui::Text("Camera: [TAB] to unlock cursor");
            ImGui::End();

            rlImGuiEnd();

        EndDrawing();
    }

    robot.Unload();
    comms.Disconnect();
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}