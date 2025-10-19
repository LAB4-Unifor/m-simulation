#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

// Changed from <imgui.h> to local includes
#include "imgui.h"
#include "implot.h"
#include <SDL.h>
#include <array>
#include <cstring>
#include "camera.h"
#include "robot_arm.h"
#include "renderer.h"
#include "lighting_system.h"
#include "performance_monitor.h"
#include "robot_communication.h"

class GUIManager {
public:
    GUIManager();
    ~GUIManager();
    
    bool initialize(SDL_Window* window, SDL_GLContext gl_context);
    void handleEvent(const SDL_Event& event);
    void newFrame();
    void render();
    
    void showMainMenuBar();
    void showDisplayControls(Camera& camera, Renderer& renderer);
    void showLightingControls(LightingSystem& lightingSystem);
    void showJointControls(RobotArm& robot);
    void showRobotStatus(const RobotArm& robot);
    void showPerformanceMonitor(PerformanceMonitor& perfMonitor);
    void showCameraControls(Camera& camera);
    void showCommunicationControls(RobotCommunication& comm);
    void showDataLoggingControls(RobotCommunication& comm);
    
    bool isBackgroundEnabled() const { return background_enabled; }
    bool isWireframeEnabled() const { return wireframe_enabled; }
    bool isSkeletonEnabled() const { return skeleton_enabled; }
    bool isGridEnabled() const { return grid_enabled; }
    glm::vec3 getBackgroundColor() const { return bg_color; }
    
    bool isPBREnabled() const { return pbr_enabled; }
    glm::vec3 getLightPosition() const { return light_position; }
    glm::vec3 getLightColor() const { return light_color; }
    float getAmbientStrength() const { return ambient_strength; }
    float getSpecularStrength() const { return specular_strength; }
    float getPointSize() const { return point_size; }

    // Public setters for communication and logging parameters
    void setServerAddress(const char* address) { strncpy(serverAddress, address, sizeof(serverAddress) - 1); serverAddress[sizeof(serverAddress) - 1] = '\0'; }
    void setServerPort(int port) { serverPort = port; }
    void setLogFilename(const char* filename) { strncpy(logFilename, filename, sizeof(logFilename) - 1); logFilename[sizeof(logFilename) - 1] = '\0'; }

    // Public getters for communication and logging parameters
    const char* getServerAddress() const { return serverAddress; }
    int getServerPort() const { return serverPort; }
    const char* getLogFilename() const { return logFilename; }
    
private:
    bool background_enabled;
    bool auto_rotate_enabled;
    bool wireframe_enabled;
    bool skeleton_enabled;
    bool grid_enabled;
    bool screen_space_pan_enabled;
    float point_size;
    glm::vec3 bg_color;
    
    bool pbr_enabled;
    glm::vec3 light_position;
    glm::vec3 light_color;
    float ambient_strength;
    float specular_strength;
    glm::vec3 ambient_light_color;
    float ambient_light_intensity;
    
    float fps_history[120];
    float frame_time_history[120];
    int history_offset;
    float current_fps;
    float current_frame_time;
    
    bool show_display_controls;
    bool show_lighting_controls;
    bool show_joint_controls;
    bool show_robot_status;
    bool show_performance;
    bool show_camera_controls;
    bool show_communication_controls;
    bool show_data_logging;
    
    char serverAddress[256];
    int serverPort;
    char logFilename[256];
    
    void updatePerformanceMetrics(float delta_time);
};

#endif