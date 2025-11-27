#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"

#include "robot_arm.h"
#include "robot_communication.h"
#include <string>

int main() {
    // 1. Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Mitsubishi RV-2SDB Simulation");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    // 2. Camera Setup (Use FREE Camera for easier navigation)
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 5.0f }; // Closer start
    camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };   // Look at robot center (approx)
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // 3. Load Robot
    RobotArm robot;
    robot.Init("assets/Robot.glb");

    RobotCommunication comms;
    comms.Connect("127.0.0.1", 502);

    // Control Variables
    bool manualControl = true;
    float modelScale = 10.0f; // Start BIG so we can see it
    
    // Disable Cursor so we can look around with mouse (press ESC to unlock)
    DisableCursor(); 

    // Main Loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        // --- Input Handling ---
        // Toggle Mouse Cursor with TAB to use UI
        if (IsKeyPressed(KEY_TAB)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }

        // Camera Logic
        if (IsCursorHidden()) {
            UpdateCamera(&camera, CAMERA_FREE); // WASD to Move, Mouse to Look
        }

        // Robot Update
        robot.Update(dt);

        // --- Draw ---
        BeginDrawing();
            ClearBackground({ 40, 40, 40, 255 }); // Dark Grey Background

            BeginMode3D(camera);
                // Draw Floor Grid
                DrawGrid(20, 1.0f); 
                
                // Draw Robot (Pass the scale)
                robot.Draw(modelScale);
                
                // Draw a red sphere at 0,0,0 to confirm where the center is
                DrawSphere({0,0,0}, 0.1f, RED);
            EndMode3D();

            // --- GUI ---
            rlImGuiBegin();

            // Instructions
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Controls:");
            ImGui::Text("[TAB] to Toggle Mouse (Camera vs UI)");
            ImGui::Text("[W,A,S,D] Move Camera");
            ImGui::Text("[Mouse] Look Around");
            ImGui::Text("[Shift] Move Fast");
            ImGui::End();

            // Controls
            ImGui::Begin("Mitsubishi RV-2SDB Settings");
            
            ImGui::SeparatorText("Visuals");
            ImGui::SliderFloat("Model Scale", &modelScale, 0.1f, 50.0f);
            
            ImGui::SeparatorText("Joints");
            // Mitsubishi Limits
            const float limits[6][2] = {
                {-240, 240}, {-120, 120}, {0, 160}, 
                {-200, 200}, {-120, 120}, {-360, 360}
            };

            for (int i = 0; i < 6; i++) {
                float angle = robot.GetJointAngle(i);
                std::string label = "Joint " + std::to_string(i + 1);
                if (ImGui::SliderFloat(label.c_str(), &angle, limits[i][0], limits[i][1])) {
                    robot.SetJointAngle(i, angle);
                }
            }

            ImGui::End();
            rlImGuiEnd();

        EndDrawing();
    }

    // Cleanup
    robot.Unload();
    comms.Disconnect();
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}