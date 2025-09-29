#include "gui_manager.h"
#include "SDL_video.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>

GuiManager::GuiManager()
    : background_enabled(true),
      auto_rotate_enabled(false),
      wireframe_enabled(false),
      skeleton_enabled(false),
      grid_enabled(true),
      screen_space_pan_enabled(true),
      point_size(1.0f),
      bg_color(0.1f, 0.1f, 0.1f),
      lighting_enabled(true),
      light_position(5.0f, 5.0f, 5.0f),
      light_color(1.0f, 1.0f, 1.0f),
      ambient_strength(0.3f),
      specular_strength(0.5f),
      simulation_mode(false),
      communication(nullptr) {
    
    // Initialize communication
    communication = new RobotCommunicationSim();
}

GuiManager::~GuiManager() {
    delete communication;
}

void GuiManager::initialize() {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable keyboard controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer bindings
    ImGui_ImplOpenGL3_Init("#version 430");
}

void GuiManager::render(Camera& camera, RobotArm& robot) {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // Main window
    ImGui::Begin("Robot Simulator Controls", nullptr, ImGuiWindowFlags_MenuBar);
    
    // Display controls
    showDisplayControls(camera);
    
    // Lighting controls
    showLightingControls();
    
    // Joint controls
    showJointControls(robot);
    
    // Robot status
    showRobotStatus(robot);
    
    // Performance monitor
    showPerformanceMonitor();
    
    // Camera controls
    showCameraControls(camera);
    
    ImGui::End();

    // Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Update the ImGui state for multiple viewports
    if (ImGui::GetIO().ConfigFlags) {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
}

void GuiManager::showDisplayControls(Camera& camera) {
    ImGui::Text("Display Controls");
    ImGui::Checkbox("Background", &background_enabled);
    ImGui::Checkbox("Auto Rotate", &auto_rotate_enabled);
    ImGui::Checkbox("Wireframe", &wireframe_enabled);
    ImGui::Checkbox("Skeleton", &skeleton_enabled);
    ImGui::Checkbox("Grid", &grid_enabled);
    ImGui::Checkbox("Screen Space Pan", &screen_space_pan_enabled);
    ImGui::SliderFloat("Point Size", &point_size, 0.5f, 5.0f);
    ImGui::ColorEdit3("Background Color", (float*)&bg_color);
}

void GuiManager::showLightingControls() {
    ImGui::Text("Lighting Controls");
    ImGui::Checkbox("Enable Lighting", &lighting_enabled);
    ImGui::SliderFloat3("Light Position", (float*)&light_position, -10.0f, 10.0f);
    ImGui::ColorEdit3("Light Color", (float*)&light_color);
    ImGui::SliderFloat("Ambient Strength", &ambient_strength, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular Strength", &specular_strength, 0.0f, 1.0f);
}

void GuiManager::showJointControls(RobotArm& robot) {
    ImGui::Text("Joint Controls");
    const auto& joints = robot.getJoints();
    for (int i = 0; i < 6; ++i) {
        char label[16];
        snprintf(label, sizeof(label), "J%d", i+1);
        float angle = joints[i].getCurrentAngle();
        float min = joints[i].getMinAngle();
        float max = joints[i].getMaxAngle();
        
        // Draw slider with limit indicators
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::SliderFloat(label, &angle, min, max);
        ImGui::PopStyleColor();
        
        // Add limit indicators
        ImGui::SameLine();
        ImGui::Text("(%.1f-%.1f)", min, max);
        
        // Visual limit warnings
        if (angle <= min + 5.0f || angle >= max - 5.0f) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "!");
        }
        ImGui::PopID();
    }
}

void GuiManager::showRobotStatus(const RobotArm& robot) {
    ImGui::Text("Robot Status");
    ImGui::Text("End Effector Position: (0.0, 0.0, 0.0)"); // Placeholder
    ImGui::Text("J1: %.1f°", robot.getJoints()[0].getCurrentAngle());
    ImGui::Text("J2: %.1f°", robot.getJoints()[1].getCurrentAngle());
    ImGui::Text("J3: %.1f°", robot.getJoints()[2].getCurrentAngle());
    ImGui::Text("J4: %.1f°", robot.getJoints()[3].getCurrentAngle());
    ImGui::Text("J5: %.1f°", robot.getJoints()[4].getCurrentAngle());
    ImGui::Text("J6: %.1f°", robot.getJoints()[5].getCurrentAngle());
}

void GuiManager::showPerformanceMonitor() {
    ImGui::Text("Performance Monitor");
    
    // FPS and Frame Time
    ImGui::Text("FPS: %.1f", performance_monitor.getCurrentFPS());
    ImGui::Text("Frame Time: %.2f ms", performance_monitor.getCurrentFrameTime());
    
    // Draw Calls and Vertices
    ImGui::Text("Draw Calls: %d", performance_monitor.getDrawCalls());
    ImGui::Text("Vertices: %d", performance_monitor.getVerticesRendered());
    
    // Memory Usage (simulated)
    ImGui::Text("Memory: %.2f MB", 150.5f);
    
    // Performance indicators
    float fps = performance_monitor.getCurrentFPS();
    if (fps < 30.0f) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WARNING: Low FPS");
    } else if (fps < 45.0f) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Warning: Below target");
    } else {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Good: Target performance");
    }
}

void GuiManager::showCameraControls(Camera& camera) {
    ImGui::Text("Camera Controls");
    ImGui::SliderFloat("Radius", &camera.getRadius(), 1.0f, 10.0f);
    ImGui::SliderFloat("FOV", &camera.getFov(), 10.0f, 120.0f);
    ImGui::Checkbox("Auto Rotate", &camera.getAutoRotateEnabled());
    
    // Communication mode toggle
    ImGui::Checkbox("Simulation Mode", &simulation_mode);
    if (simulation_mode) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "(Active)");
    }
}

void GuiManager::setSimulationMode(bool enabled) {
    simulation_mode = enabled;
    communication->setSimulationMode(enabled);
}

bool GuiManager::isSimulationMode() const {
    return simulation_mode;
}
