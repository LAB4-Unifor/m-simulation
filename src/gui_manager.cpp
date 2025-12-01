#include "gui_manager.h"
#include "imgui.h"
#include "raylib.h"
#include <string>

void GuiManager::Draw(RobotController& controller, RobotCommunication& comms) {
    ImGui::Begin("Mitsubishi Controller - TCC Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // --- Header ---
    ImGui::Text("System: RV-2SDB Simulation");
    if (controller.IsInCollision()) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "WARNING: COLLISION DETECTED!");
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Motion Halted via Safety Protocol");
    } else {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: OPERATIONAL");
    }
    ImGui::Separator();

    // --- Tab Bar para Modos ---
    if (ImGui::BeginTabBar("Modes")) {
        
        // ABA 1: MANUAL (Cinemática Direta)
        if (ImGui::BeginTabItem("Manual Control")) {
            ImGui::Spacing();
            if (controller.IsDemoActive()) ImGui::BeginDisabled();

            // Botões Rápidos
            if (ImGui::Button("HOME")) controller.SetAllTargets({0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f}); 
            ImGui::SameLine();
            if (ImGui::Button("READY")) controller.SetAllTargets({0.0f, 20.0f, 80.0f, 0.0f, -80.0f, 0.0f});

            ImGui::Spacing();
            ImGui::SeparatorText("Joint Angles (Degrees)");

            for (int i = 0; i < 6; i++) {
                float angle = controller.GetTargetAngle(i);
                std::string label = "J" + std::to_string(i + 1);
                
                // Slider respeita os limites definidos no RobotController.h
                if (ImGui::SliderFloat(label.c_str(), &angle, 
                    RobotController::LIMITS[i][0], RobotController::LIMITS[i][1])) {
                    controller.SetJointTarget(i, angle);
                }
            }
            if (controller.IsDemoActive()) ImGui::EndDisabled();
            ImGui::EndTabItem();
        }

        // ABA 2: AUTOMÁTICO (Cinemática Inversa)
        if (ImGui::BeginTabItem("Inverse Kinematics")) {
            ImGui::Spacing();
            ImGui::TextWrapped("Implementation of algorithms described in Section 4.5.1 (FABRIK/Geometric Solver).");
            ImGui::Spacing();

            static float targetPos[3] = { 0.3f, 0.4f, 0.0f }; // X, Y, Z
            
            ImGui::Text("Target Position (Meters):");
            ImGui::DragFloat3("XYZ", targetPos, 0.01f, -0.8f, 0.8f);

            if (ImGui::Button("MOVE TO TARGET", ImVec2(-1, 40))) {
                controller.SetTargetPosition({targetPos[0], targetPos[1], targetPos[2]});
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            if (controller.IsDemoActive()) {
                if (ImGui::Button("STOP DEMO SEQ", ImVec2(-1, 0))) controller.StopDemo();
            } else {
                if (ImGui::Button("RUN IK DEMO SEQ", ImVec2(-1, 0))) controller.StartDemo();
            }

            ImGui::EndTabItem();
        }

        // ABA 3: VISUAL
        if (ImGui::BeginTabItem("Visual Settings")) {
            ImGui::ColorEdit3("Background", bgColor);
            ImGui::Checkbox("Wireframe Mode", &showWireframe);
            ImGui::Checkbox("Show Work Envelope", &showEnvelope);
            ImGui::SliderFloat("Scale", &modelScale, 0.5f, 5.0f);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    // --- Overlay de Status ---
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::TextColored(ImVec4(0,1,0,1), "FPS: %d", GetFPS());
    ImGui::Text("TCP/IP: %s", comms.IsConnected() ? "CONNECTED" : "LISTENING (Port 10001)");
    ImGui::End();
}