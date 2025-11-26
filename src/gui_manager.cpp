#include "gui_manager.h"
#include <iostream>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

GUIManager::GUIManager() 
    : background_enabled(true),
      auto_rotate_enabled(false),
      wireframe_enabled(false),
      skeleton_enabled(false),
      grid_enabled(true),
      screen_space_pan_enabled(true),
      point_size(1.0f),
      bg_color(0.1f, 0.1f, 0.1f),
      pbr_enabled(true),
      light_position(2.0f, 5.0f, 2.0f),
      light_color(1.0f, 1.0f, 1.0f),
      ambient_strength(0.2f),
      specular_strength(0.5f),
      ambient_light_color(0.1f, 0.1f, 0.1f),
      ambient_light_intensity(0.1f),
      history_offset(0),
      current_fps(0.0f),
      current_frame_time(0.0f),
      show_display_controls(true),
      show_lighting_controls(true),
      show_joint_controls(true),
      show_robot_status(true),
      show_performance(true),
      show_camera_controls(true),
      show_communication_controls(true),
      show_data_logging(true) {
    
    for (int i = 0; i < 120; i++) {
        fps_history[i] = 0.0f;
        frame_time_history[i] = 0.0f;
    }
    
    strcpy(serverAddress, "192.168.1.100");
    serverPort = 502;
    strcpy(logFilename, "robot_log.csv");
}

GUIManager::~GUIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

bool GUIManager::initialize(SDL_Window* window, SDL_GLContext gl_context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Remove docking and viewports for now
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    ImGui::StyleColorsDark();
    
    // Remove viewport style adjustments
    // ImGuiStyle& style = ImGui::GetStyle();
    // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //     style.WindowRounding = 0.0f;
    //     style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    // }
    
    if (!ImGui_ImplSDL2_InitForOpenGL(window, gl_context)) {
        std::cerr << "Failed to initialize ImGui SDL2 backend!" << std::endl;
        return false;
    }
    
    if (!ImGui_ImplOpenGL3_Init("#version 430")) {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend!" << std::endl;
        return false;
    }
    
    return true;
}

void GUIManager::handleEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void GUIManager::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void GUIManager::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
}

void GUIManager::showMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem("Display Controls", NULL, &show_display_controls);
            ImGui::MenuItem("Lighting Controls", NULL, &show_lighting_controls);
            ImGui::MenuItem("Joint Controls", NULL, &show_joint_controls);
            ImGui::MenuItem("Robot Status", NULL, &show_robot_status);
            ImGui::MenuItem("Performance Monitor", NULL, &show_performance);
            ImGui::MenuItem("Camera Controls", NULL, &show_camera_controls);
            ImGui::MenuItem("Communication Controls", NULL, &show_communication_controls);
            ImGui::MenuItem("Data Logging", NULL, &show_data_logging);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Presets")) {
            if (ImGui::MenuItem("Home Position")) {}
            if (ImGui::MenuItem("Extended Position")) {}
            if (ImGui::MenuItem("Folded Position")) {}
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Communication")) {
            if (ImGui::MenuItem("Connect")) {}
            if (ImGui::MenuItem("Disconnect")) {}
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void GUIManager::showDisplayControls(Camera& camera, Renderer& renderer) {
    if (!show_display_controls) return;
    
    ImGui::Begin("Display Controls", &show_display_controls);
    
    ImGui::Checkbox("Background", &background_enabled);
    ImGui::SameLine();
    ImGui::ColorEdit3("BG Color", &bg_color[0], ImGuiColorEditFlags_NoInputs);
    
    ImGui::Checkbox("Auto Rotate", &auto_rotate_enabled);
    if (auto_rotate_enabled) {
        static float rotate_speed = 1.0f;
        ImGui::SliderFloat("Rotate Speed", &rotate_speed, 0.1f, 5.0f);
        camera.setAutoRotate(true, rotate_speed);
    } else {
        camera.setAutoRotate(false);
    }
    
    ImGui::Checkbox("Wireframe", &wireframe_enabled);
    ImGui::Checkbox("Skeleton", &skeleton_enabled);
    
    ImGui::Checkbox("Grid", &grid_enabled);
    renderer.getGridRenderer().setEnabled(grid_enabled);
    
    ImGui::Checkbox("Screen Space Pan", &screen_space_pan_enabled);
    ImGui::SliderFloat("Point Size", &point_size, 0.1f, 5.0f);
    
    ImGui::End();
}

void GUIManager::showLightingControls(LightingSystem& lightingSystem) {
    if (!show_lighting_controls) return;
    
    ImGui::Begin("Lighting Controls", &show_lighting_controls);
    
    ImGui::Checkbox("Enable PBR", &pbr_enabled);
    lightingSystem.setUsePBR(pbr_enabled);
    
    ImGui::Separator();
    ImGui::Text("Ambient Light");
    ImGui::ColorEdit3("Ambient Color", &ambient_light_color[0]);
    ImGui::SliderFloat("Ambient Intensity", &ambient_light_intensity, 0.0f, 1.0f);
    lightingSystem.setAmbientLight(ambient_light_color, ambient_light_intensity);
    
    ImGui::Separator();
    ImGui::Text("Directional Lights");
    
    for (int i = 0; i < lightingSystem.getLightCount(); i++) {
        ImGui::PushID(i);
        LightingSystem::Light& light = lightingSystem.getLight(i);
        
        ImGui::Text("Light %d", i + 1);
        ImGui::Checkbox("Enabled", &light.enabled);
        ImGui::ColorEdit3("Color", &light.color[0]);
        ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
        ImGui::SliderFloat3("Position", &light.position[0], -10.0f, 10.0f);
        
        ImGui::Separator();
        ImGui::PopID();
    }
    
    if (ImGui::Button("Add Light") && lightingSystem.getLightCount() < 4) {
        LightingSystem::Light newLight;
        newLight.position = glm::vec3(2.0f, 5.0f, 2.0f);
        newLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
        newLight.intensity = 1.0f;
        lightingSystem.addLight(newLight);
    }
    
    if (ImGui::Button("Remove Light") && lightingSystem.getLightCount() > 0) {
        lightingSystem.removeLight(lightingSystem.getLightCount() - 1);
    }
    
    ImGui::End();
}

void GUIManager::showJointControls(RobotArm& robot) {
    if (!show_joint_controls) return;
    
    ImGui::Begin("Joint Controls", &show_joint_controls);
    
    const std::array<RobotJoint, 6>& joints = robot.getJoints();
    const char* joint_names[] = {"J1 - Base", "J2 - Shoulder", "J3 - Elbow", 
                                "J4 - Wrist Roll", "J5 - Wrist Pitch", "J6 - Wrist Yaw"};
    
    for (int i = 0; i < 6; i++) {
        ImGui::PushID(i);
        
        float current_angle = joints[i].getCurrentAngle();
        float min_angle = joints[i].getMinAngle();
        float max_angle = joints[i].getMaxAngle();
        
        ImGui::Text("%s", joint_names[i]);
        
        float normalized = (current_angle - min_angle) / (max_angle - min_angle);
        ImVec4 color;
        if (normalized < 0.1f || normalized > 0.9f) {
            color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        } else if (normalized < 0.2f || normalized > 0.8f) {
            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        } else {
            color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        }
        
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.3f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(0.3f, 0.7f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(0.3f, 0.9f, 0.9f));
        
        if (ImGui::SliderFloat("Angle", &current_angle, min_angle, max_angle, "%.2f deg")) {
            robot.setJointAngle(i, current_angle, true);
        }
        
        ImGui::PopStyleColor(4);
        
        ImGui::Text("Current: %.2f deg (Min: %.2f, Max: %.2f)", 
                   current_angle, min_angle, max_angle);
        
        ImGui::Separator();
        ImGui::PopID();
    }
    
    if (ImGui::Button("Home Position")) {
        robot.resetToHomePosition(true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Extended Position")) {
        std::array<float, 6> extended = {0.0f, 0.0f, 160.0f, 0.0f, 0.0f, 0.0f};
        robot.setJointAngles(extended, true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Folded Position")) {
        std::array<float, 6> folded = {0.0f, -90.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        robot.setJointAngles(folded, true);
    }
    
    ImGui::End();
}

void GUIManager::showRobotStatus(const RobotArm& robot) {
    if (!show_robot_status) return;
    
    ImGui::Begin("Robot Status", &show_robot_status);
    
    const std::array<RobotJoint, 6>& joints = robot.getJoints();
    const char* joint_names[] = {"J1 - Base", "J2 - Shoulder", "J3 - Elbow", 
                                "J4 - Wrist Roll", "J5 - Wrist Pitch", "J6 - Wrist Yaw"};
    
    ImGui::Text("Joint Angles:");
    for (int i = 0; i < 6; i++) {
        float angle = joints[i].getCurrentAngle();
        float min_angle = joints[i].getMinAngle();
        float max_angle = joints[i].getMaxAngle();
        
        float normalized = (angle - min_angle) / (max_angle - min_angle);
        if (normalized < 0.1f || normalized > 0.9f) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        } else if (normalized < 0.2f || normalized > 0.8f) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        
        ImGui::Text("%s: %.2f deg", joint_names[i], angle);
        ImGui::PopStyleColor();
    }
    
    ImGui::Separator();
    
    glm::mat4 end_effector = robot.getEndEffectorTransform();
    ImGui::Text("End Effector Position:");
    ImGui::Text("X: %.3f m", end_effector[3][0]);
    ImGui::Text("Y: %.3f m", end_effector[3][1]);
    ImGui::Text("Z: %.3f m", end_effector[3][2]);
    
    ImGui::End();
}

void GUIManager::showPerformanceMonitor(PerformanceMonitor& perfMonitor) {
    if (!show_performance) return;
    
    perfMonitor.renderGUI();
}

void GUIManager::showCameraControls(Camera& camera) {
    if (!show_camera_controls) return;
    
    ImGui::Begin("Camera Controls", &show_camera_controls);
    
    glm::vec3 position = camera.getPosition();
    glm::vec3 target = camera.getTarget();
    
    ImGui::Text("Camera Position:");
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", position.x, position.y, position.z);
    
    ImGui::Text("Camera Target:");
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", target.x, target.y, target.z);
    
    ImGui::Separator();
    
    if (ImGui::Button("Reset Camera")) {
        camera.setTarget(glm::vec3(0.0f, 1.0f, 0.0f));
        camera.setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Top View")) {
        camera.setTarget(glm::vec3(0.0f, 1.0f, 0.0f));
        camera.setPosition(glm::vec3(0.0f, 10.0f, 0.1f));
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Front View")) {
        camera.setTarget(glm::vec3(0.0f, 1.0f, 0.0f));
        camera.setPosition(glm::vec3(0.0f, 2.0f, 10.0f));
    }
    
    ImGui::End();
}

void GUIManager::showCommunicationControls(RobotCommunication& comm) {
    if (!show_communication_controls) return;
    
    ImGui::Begin("Communication Controls", &show_communication_controls);
    
    auto state = comm.getConnectionState();
    ImGui::Text("Status: ");
    ImGui::SameLine();
    
    switch (state) {
        case RobotCommunication::ConnectionState::DISCONNECTED:
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "DISCONNECTED");
            break;
        case RobotCommunication::ConnectionState::CONNECTING:
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "CONNECTING");
            break;
        case RobotCommunication::ConnectionState::CONNECTED:
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "CONNECTED");
            break;
        case RobotCommunication::ConnectionState::ERROR:
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "ERROR");
            break;
    }
    
    ImGui::Separator();
    ImGui::Text("Connection Settings:");
    
    ImGui::InputText("Server Address", serverAddress, sizeof(serverAddress));
    ImGui::InputInt("Port", &serverPort);
    
    if (state == RobotCommunication::ConnectionState::DISCONNECTED || 
        state == RobotCommunication::ConnectionState::ERROR) {
        if (ImGui::Button("Connect")) {
            comm.connect(serverAddress, serverPort);
        }
    } else {
        if (ImGui::Button("Disconnect")) {
            comm.disconnect();
        }
    }
    
    ImGui::Separator();
    ImGui::Text("Simulation Mode:");
    
    auto currentMode = comm.getSimulationMode();
    const char* modes[] = { "Manual", "Playback", "Real-time Sync", "Simulation Only" };
    
    for (int i = 0; i < IM_ARRAYSIZE(modes); i++) {
        if (ImGui::RadioButton(modes[i], currentMode == static_cast<RobotCommunication::SimulationMode>(i))) {
            comm.setSimulationMode(static_cast<RobotCommunication::SimulationMode>(i));
        }
        if (i < IM_ARRAYSIZE(modes) - 1) {
            ImGui::SameLine();
        }
    }
    
    ImGui::End();
}

void GUIManager::showDataLoggingControls(RobotCommunication& comm) {
    if (!show_data_logging) return;
    
    ImGui::Begin("Data Logging", &show_data_logging);
    
    ImGui::Text("Logging: ");
    ImGui::SameLine();
    
    if (comm.isLogging()) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "ACTIVE");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "INACTIVE");
    }
    
    ImGui::Separator();
    ImGui::InputText("Log File", logFilename, sizeof(logFilename));
    
    if (!comm.isLogging()) {
        if (ImGui::Button("Start Logging")) {
            comm.startLogging(logFilename);
        }
    } else {
        if (ImGui::Button("Stop Logging")) {
            comm.stopLogging();
        }
    }
    
    ImGui::Separator();
    ImGui::Text("Data Export/Import:");
    
    static char exportFilename[256] = "trajectory.csv";
    static char importFilename[256] = "trajectory.csv";
    
    ImGui::InputText("Export File", exportFilename, sizeof(exportFilename));
    if (ImGui::Button("Export Current Trajectory")) {
        // Export implementation would go here
    }
    
    ImGui::InputText("Import File", importFilename, sizeof(importFilename));
    if (ImGui::Button("Import Trajectory")) {
        // Import implementation would go here
    }
    
    ImGui::End();
}

void GUIManager::updatePerformanceMetrics(float delta_time) {
    if (delta_time > 0.0f) {
        current_fps = 1.0f / delta_time;
        current_frame_time = delta_time * 1000.0f;
    }
    
    fps_history[history_offset] = current_fps;
    frame_time_history[history_offset] = current_frame_time;
    history_offset = (history_offset + 1) % IM_ARRAYSIZE(fps_history);
}
