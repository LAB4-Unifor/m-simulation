#pragma once
#include <glm/glm.hpp>
#include "camera.h"
#include "robot.h"
#include "performance_monitor.h"
#include "robot_communication.h"

class GuiManager {
public:
    GuiManager();
    ~GuiManager();
    void initialize();
    void render(Camera& camera, RobotArm& robot);
    
    // Display controls
    bool isBackgroundEnabled() const { return background_enabled; }
    bool isAutoRotateEnabled() const { return auto_rotate_enabled; }
    bool isWireframeEnabled() const { return wireframe_enabled; }
    bool isSkeletonEnabled() const { return skeleton_enabled; }
    bool isGridEnabled() const { return grid_enabled; }
    bool isScreenSpacePanEnabled() const { return screen_space_pan_enabled; }
    float getPointSize() const { return point_size; }
    glm::vec3 getBackgroundColor() const { return bg_color; }
    
    // Lighting controls
    bool isLightingEnabled() const { return lighting_enabled; }
    glm::vec3 getLightPosition() const { return light_position; }
    glm::vec3 getLightColor() const { return light_color; }
    float getAmbientStrength() const { return ambient_strength; }
    float getSpecularStrength() const { return specular_strength; }
    
    // Communication
    void setSimulationMode(bool enabled);
    bool isSimulationMode() const;
    
private:
    // Display controls
    bool background_enabled;
    bool auto_rotate_enabled;
    bool wireframe_enabled;
    bool skeleton_enabled;
    bool grid_enabled;
    bool screen_space_pan_enabled;
    float point_size;
    glm::vec3 bg_color;
    
    // Lighting controls
    bool lighting_enabled;
    glm::vec3 light_position;
    glm::vec3 light_color;
    float ambient_strength;
    float specular_strength;
    
    // Communication
    bool simulation_mode;
    RobotCommunication* communication;
    
    void showDisplayControls(Camera& camera);
    void showLightingControls();
    void showJointControls(RobotArm& robot);
    void showRobotStatus(const RobotArm& robot);
    void showPerformanceMonitor();
    void showCameraControls(Camera& camera);
};