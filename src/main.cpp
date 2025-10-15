#include <iostream>



/*Serial example
  // create an asio io_service object
  boost::asio::io_service io_service;

  // create a serial port object
  boost::asio::serial_port serial_port(io_service);

  // open the serial port
  serial_port.open("/dev/ttyUSB0");

  // set serial port options
  serial_port.set_option(boost::asio::serial_port_base::baud_rate(9600));
  serial_port.set_option(boost::asio::serial_port_base::character_size(8));
  serial_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
  serial_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
  serial_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

  // create a buffer to store the received data
  char buffer[1024];

  // read data from the serial port
  size_t n = boost::asio::read(serial_port, boost::asio::buffer(buffer, 1024));

  // convert the received data to binary
  std::string binary_data;
  for (size_t i = 0; i < n; i++) {
    for (int j = 7; j >= 0; j--) {
      if (buffer[i] & (1 << j)) {
        binary_data.push_back('1');
      } else {
        binary_data.push_back('0');
      }
    }
  }

  // print the binary data
  std::cout << "Received data in binary format: " << binary_data << std::endl;

  // close the serial port
  serial_port.close();*/


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
    
    // strcpy(guiManager.serverAddress, "192.168.1.100");
    // guiManager.serverPort = 502;
    // strcpy(guiManager.logFilename, "robot_log.csv");
    guiManager.setServerAddress("192.168.1.100");
    guiManeger.setServerPort(502);
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
                        // Use getter methods instead of direct access
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
        
        glPointSize(guiManager.isWireframeEnabled() ? guiManager.point_size : 1.0f);
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
