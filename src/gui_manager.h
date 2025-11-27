#pragma once
#include "robot_controller.h"
#include "robot_communication.h"

class GuiManager {
public:
    // UI State
    float modelScale = 2.0f;
    bool showWireframe = false;
    bool showEnvelope = false;
    float bgColor[3] = { 0.12f, 0.12f, 0.12f };

    void Draw(RobotController& controller, RobotCommunication& comms);
};