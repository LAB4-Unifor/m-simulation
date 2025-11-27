#include "gui_manager.h"
#include "imgui.h"
#include "raylib.h"
#include <string>

void GuiManager::Draw(RobotController& controller, RobotCommunication& comms) {
    ImGui::Begin("Mitsubishi Controller", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // --- Simulation Mode ---
    ImGui::SeparatorText("Simulation Mode");
    if (ImGui::Button("HOME")) controller.SetAllTargets({0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f}); 
    ImGui::SameLine();
    if (ImGui::Button("ZERO")) controller.SetAllTargets({0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    ImGui::SameLine();
    if (ImGui::Button("READY")) controller.SetAllTargets({0.0f, 30.0f, 60.0f, 0.0f, -90.0f, 0.0f});

    ImGui::Spacing();

    if (controller.IsDemoActive()) {
        if (ImGui::Button("STOP DEMO", ImVec2(-1, 0))) controller.StopDemo();
    } else {
        if (ImGui::Button("RUN TEST DEMO", ImVec2(-1, 0))) controller.StartDemo();
    }

    // --- Visuals ---
    ImGui::SeparatorText("Visual Settings");
    ImGui::ColorEdit3("Background", bgColor);
    ImGui::Checkbox("Wireframe Mode", &showWireframe);
    ImGui::Checkbox("Show Work Envelope", &showEnvelope);
    ImGui::SliderFloat("Scale", &modelScale, 0.5f, 5.0f);

    // --- Joints ---
    ImGui::SeparatorText("Joint Control");
    if (controller.IsDemoActive()) ImGui::BeginDisabled();

    for (int i = 0; i < 6; i++) {
        float angle = controller.GetTargetAngle(i); // Use target to avoid slider fighting
        std::string label = "J" + std::to_string(i + 1);
        
        // Use limits from Controller
        if (ImGui::SliderFloat(label.c_str(), &angle, 
            RobotController::LIMITS[i][0], RobotController::LIMITS[i][1])) {
            controller.SetJointTarget(i, angle);
        }
    }

    if (controller.IsDemoActive()) ImGui::EndDisabled();
    ImGui::End();

    // --- Status Overlay ---
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::TextColored(ImVec4(0,1,0,1), "FPS: %d", GetFPS());
    ImGui::Text("Comm: %s", comms.IsConnected() ? "CONNECTED" : "WAITING...");
    ImGui::End();
}