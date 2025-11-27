#include "raylib.h"
#include "rlImGui.h"
#include "robot_controller.h"
#include "robot_model.h"
#include "robot_communication.h"
#include "gui_manager.h"

int main() {
    // 1. Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Mitsubishi RV-2SDB Simulation (Modular)");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    // 2. Camera Setup
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 6.0f, 6.0f, 6.0f };
    camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // 3. Instantiate Components
    RobotController controller;   // Handles logic, interpolation, and limits
    RobotModel model;             // Handles drawing (procedural or 3D mesh)
    RobotCommunication comms;     // Handles Socket Server
    GuiManager gui;               // Handles ImGui Interface

    // 4. Component Setup
    model.Init("assets/Robot.glb"); // Try to load GLB, falls back to geometric if missing
    
    // Starts the socket server on port 10001 (IP argument is ignored in provided impl, but required by signature)
    comms.Connect("0.0.0.0", 10001); 

    DisableCursor(); // Start with camera control active

    // 5. Main Loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- Input Handling ---
        if (IsKeyPressed(KEY_TAB)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }

        // Only move camera if cursor is locked
        if (IsCursorHidden()) {
            UpdateCamera(&camera, CAMERA_FREE);
        }

        // --- Logic Update ---
        controller.Update(dt);

        // Sync: Send current angles to the network thread so clients using "GETPOS" see real data
        comms.SendJointAngles(controller.GetAllAngles());

        // --- Drawing ---
        BeginDrawing();
            
            // Convert GUI float color (0.0-1.0) to Raylib Color (0-255)
            Color clearCol = { 
                (unsigned char)(gui.bgColor[0] * 255), 
                (unsigned char)(gui.bgColor[1] * 255), 
                (unsigned char)(gui.bgColor[2] * 255), 
                255 
            };
            ClearBackground(clearCol);

            BeginMode3D(camera);
                DrawGrid(20, 1.0f);
                
                // Draw the Robot
                // We get angles from Controller and settings from GUI Manager
                model.Draw(controller.GetAllAngles(), gui.modelScale, gui.showWireframe);

                // Draw Envelope if enabled in GUI
                if (gui.showEnvelope) {
                    // Simple visualization of reach (approximate sphere)
                    DrawSphereWires({0, 1.0f, 0}, 2.5f * gui.modelScale, 16, 16, {255, 255, 255, 50});
                }
            EndMode3D();

            // --- GUI Overlay ---
            rlImGuiBegin();
                // Pass dependencies to the GUI Manager
                gui.Draw(controller, comms);
            rlImGuiEnd();

        EndDrawing();
    }

    // 6. Cleanup
    model.Unload();
    comms.Disconnect();
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}