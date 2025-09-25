#pragma once
#include <array>
#include <string>

class RobotCommunication {
public:
    virtual ~RobotCommunication() = default;
    
    virtual bool connect(const std::string& address) = 0;
    virtual void sendJointCommand(const std::array<float, 6>& angles) = 0;
    virtual std::array<float, 6> readJointPositions() = 0;
    virtual bool isConnected() const = 0;
    
    // Simulation mode control
    virtual void setSimulationMode(bool enabled) = 0;
    virtual bool isSimulationMode() const = 0;
};