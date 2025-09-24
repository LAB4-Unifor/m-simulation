#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "renderer.h"
#include "grid_renderer.h"
#include "model_loader.h"
#include "camera.h"
#include "input_handler.h"
#include "robot.h"
#include "gui_manager.h"
#include "performance_monitor.h"
#include "robot_communication.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Set OpenGL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create window (1920x1080, resizable)
    SDL_Window* window = SDL_CreateWindow(
        "Robot Simulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1920,
        1080,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Set viewport and clear color
    glViewport(0, 0, 1920, 1080);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Enable OpenGL features
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Initialize shaders
    ShaderProgram shader;
    if (!shader.loadFromFiles("shaders/vertex.glsl", "shaders/fragment.glsl")) {
        std::cerr << "Failed to load main shaders" << std::endl;
        return 1;
    }

    // Initialize grid renderer
    GridRenderer gridRenderer;
    gridRenderer.initialize();

    // Load robot model
    ModelLoader robotModel;
    if (!robotModel.loadGLTF("assets/Yuki.glb")) {
        std::cerr << "Failed to load model: assets/Yuki.glb" << std::endl;
        return 1;
    }

    // Initialize camera
    Camera camera(
        glm::vec3(5.0f, 5.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        45.0f,
        1920.0f / 1080.0f,
        0.1f,
        100.0f
    );
    camera.setAutoRotate(true, 0.5f);

    // Initialize input handler
    InputHandler inputHandler;

    // Initialize robot arm
    RobotArm robotArm;
    std::array<float, 6> homeAngles = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    robotArm.setJointAngles(homeAngles);
    robotArm.updateKinematics();

    // Initialize GUI manager
    GuiManager guiManager;
    guiManager.initialize();

    // Initialize communication
    RobotCommunication* communication = new RobotCommunicationSim();
    communication->connect("sim://robot");

    // Main loop
    bool running = true;
    SDL_Event event;
    float lastFrameTime = 0.0f;
    float currentFrameTime = 0.0f;
    int drawCalls = 0;
    int vertices = 0;
    
    while (running) {
        currentFrameTime = SDL_GetTicks() / 1000.0f;
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_WINDOWEVENT && 
                event.window.event == SDL_WINDOWEVENT_RESIZED) {
                glViewport(0, 0, event.window.data1, event.window.data2);
                camera.setAspectRatio(static_cast<float>(event.window.data1) / event.window.data2);
            }
            inputHandler.handleEvent(event, camera);
        }

        // Update camera
        camera.update(deltaTime);

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up camera matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        
        // Render grid
        gridRenderer.render(view, projection);
        
        // Render robot model
        shader.use();
        shader.setUniform("view", view);
        shader.setUniform("projection", projection);
        shader.setUniform("lightPos", guiManager.getLightPosition());
        shader.setUniform("viewPos", camera.getPosition());
        shader.setUniform("lightColor", guiManager.getLightColor());
        shader.setUniform("objectColor", glm::vec3(0.8f, 0.8f, 0.8f));
        shader.setUniform("ambientStrength", guiManager.getAmbientStrength());
        shader.setUniform("specularStrength", guiManager.getSpecularStrength());
        shader.setUniform("shininess", 32.0f);
        
        glm::mat4 model = glm::mat4(1.0f);
        shader.setUniform("model", model);
        
        // Simulate draw calls and vertices
        drawCalls = 1; // Grid + Robot
        vertices = robotModel.getMeshes().size() * 12000;
        robotArm.render(model, view, projection);
        
        // Update performance monitor
        guiManager.updatePerformance(deltaTime, drawCalls, vertices);
        
        // Render GUI
        guiManager.render(camera, robotArm);
        
        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    delete communication;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}