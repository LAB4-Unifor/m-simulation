#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer.h"
#include "shader_program.h"
#include "camera.h"
#include "input_handler.h"
#include "robot_arm.h"
#include "gui_manager.h"
#include "lighting_system.h"
#include "animation_controller.h"
#include "robot_communication.h"

int main(int argc, char* argv[]) {
    std::cout << "Initializing Robot Simulator..." << std::endl;
    
    Renderer renderer;
    if (!renderer.initialize(1920, 1080, "Robot Simulator")) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    
    GUIManager guiManager;
    if (!guiManager.initialize(renderer.getWindow(), renderer.getGLContext())) {
        std::cerr << "Failed to initialize GUI manager!" << std::endl;
        return -1;
    }
    
    ShaderProgram pbrShader;
    if (!pbrShader.loadFromFiles("shaders/pbr_vertex.glsl", "shaders/pbr_fragment.glsl")) {
        std::cerr << "Failed to load PBR shader! Using fallback..." << std::endl;
        if (!pbrShader.loadFromFiles("shaders/vertex.glsl", "shaders/fragment.glsl")) {
            std::cerr << "Failed to load fallback shader!" << std::endl;
            return -1;
        }
    }
    
    LightingSystem lightingSystem;
    
    LightingSystem::Light mainLight;
    mainLight.position = glm::vec3(2.0f, 5.0f, 2.0f);
    mainLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    mainLight.intensity = 1.0f;
    lightingSystem.addLight(mainLight);
    
    LightingSystem::Light fillLight;
    fillLight.position = glm::vec3(-2.0f, 3.0f, -2.0f);
    fillLight.color = glm::vec3(0.5f, 0.5f, 0.8f);
    fillLight.intensity = 0.5f;
    lightingSystem.addLight(fillLight);
    
    Camera camera;
    camera.setPerspective(glm::radians(45.0f), 
                         static_cast<float>(renderer.getWidth()) / static_cast<float>(renderer.getHeight()),
                         0.1f, 100.0f);
    camera.setTarget(glm::vec3(0.0f, 1.0f, 0.0f));
    camera.setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    
    InputHandler inputHandler;
    
    RobotArm robotArm;
    robotArm.initialize();
    
    AnimationController animationController;
    
    AnimationController::Keyframe homeFrame;
    homeFrame.jointAngles = {0.0f, 0.0f, 90.0f, 0.0f, 0.0f, 0.0f};
    homeFrame.timestamp = 0.0f;
    homeFrame.name = "Home Position";
    animationController.addKeyframe(homeFrame);
    
    AnimationController::Keyframe extendedFrame;
    extendedFrame.jointAngles = {0.0f, 0.0f, 160.0f, 0.0f, 0.0f, 0.0f};
    extendedFrame.timestamp = 2.0f;
    extendedFrame.name = "Extended Position";
    animationController.addKeyframe(extendedFrame);
    
    AnimationController::Keyframe foldedFrame;
    foldedFrame.jointAngles = {0.0f, -90.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    foldedFrame.timestamp = 4.0f;
    foldedFrame.name = "Folded Position";
    animationController.addKeyframe(foldedFrame);
    
    RobotCommunication robotComm;
    
    robotComm.setConnectionCallback([&](RobotCommunication::ConnectionState state) {
        std::cout << "Connection state changed: " << static_cast<int>(state) << std::endl;
    });
    
    robotComm.setDataReceivedCallback([&](const std::array<float, 6>& angles) {
        if (robotComm.getSimulationMode() == RobotCommunication::SimulationMode::REAL_TIME_SYNC) {
            robotArm.setJointAngles(angles, true);
        }
    });
    
    robotComm.setErrorCallback([&](const std::string& error) {
        std::cerr << "Communication error: " << error << std::endl;
    });
    
    guiManager.setServerAddress("192.168.1.100");
    guiManager.setServerPort(502);
    guiManager.setLogFilename("robot_log.csv");
    
    bool quit = false;
    SDL_Event e;
    
    glEnable(GL_DEPTH_TEST);
    
    Uint32 lastTime = SDL_GetTicks();
    
    PerformanceMonitor& perfMonitor = renderer.getPerformanceMonitor();
    
    while (!quit) {
        perfMonitor.beginFrame();
        
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            
            guiManager.handleEvent(e);
            
            ImGuiIO& io = ImGui::GetIO();
            if (!io.WantCaptureMouse && !io.WantCaptureKeyboard) {
                inputHandler.handleEvent(e, camera);
                
                if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_c:
                    if (robotComm.isConnected()) {
                        robotComm.disconnect();
                    } else {
                        robotComm.connect(guiManager.getServerAddress(), guiManager.getServerPort());
                    }
                break;
            }
        }
        
        inputHandler.update(deltaTime, camera);
        camera.update(deltaTime);
        animationController.update(deltaTime, robotArm);
        robotArm.update(deltaTime);
        
        auto mode = robotComm.getSimulationMode();
        if (mode == RobotCommunication::SimulationMode::REAL_TIME_SYNC && robotComm.isConnected()) {
            auto angles = robotArm.getJointAngles();
            robotComm.sendJointAngles(angles);
        } else if (mode == RobotCommunication::SimulationMode::MANUAL && robotComm.isConnected()) {
            auto angles = robotComm.readJointAngles();
            robotArm.setJointAngles(angles, true);
        }
        
        guiManager.newFrame();
        
        guiManager.showMainMenuBar();
        guiManager.showDisplayControls(camera, renderer);
        guiManager.showLightingControls(lightingSystem);
        guiManager.showJointControls(robotArm);
        guiManager.showRobotStatus(robotArm);
        guiManager.showPerformanceMonitor(perfMonitor);
        guiManager.showCameraControls(camera);
        guiManager.showCommunicationControls(robotComm);
        guiManager.showDataLoggingControls(robotComm);
        
        animationController.renderGUI();
        
        if (guiManager.isBackgroundEnabled()) {
            glm::vec3 bgColor = guiManager.getBackgroundColor();
            glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
        } else {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }
        renderer.clear();
        
        perfMonitor.beginGPUTimer("Main Rendering");
        
        pbrShader.use();
        pbrShader.setUniform("projection", camera.getProjectionMatrix());
        pbrShader.setUniform("view", camera.getViewMatrix());
        pbrShader.setUniform("viewPos", camera.getPosition());
        
        lightingSystem.applyToShader(pbrShader);
        
        pbrShader.setUniform("material.albedo", glm::vec3(0.7f, 0.7f, 0.7f));
        pbrShader.setUniform("material.metallic", 0.0f);
        pbrShader.setUniform("material.roughness", 0.5f);
        pbrShader.setUniform("material.ao", 1.0f);
        pbrShader.setUniform("wireframeMode", guiManager.isWireframeEnabled());
        
        glPointSize(guiManager.isWireframeEnabled() ? guiManager.getPointSize() : 1.0f);
        
        if (guiManager.isGridEnabled()) {
            renderer.getGridRenderer().render(camera.getViewMatrix(), camera.getProjectionMatrix());
        }
        
        robotArm.render(pbrShader);
        
        perfMonitor.endGPUTimer("Main Rendering");
        guiManager.render();
        perfMonitor.endFrame();
        perfMonitor.update(deltaTime);
        
        renderer.swapBuffers();
        SDL_Delay(1);
    }
    
    robotComm.disconnect();
    renderer.cleanup();
    
    return 0;
}
}
