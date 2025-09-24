#include "robot_communication.h"
#include <array>
#include <iostream>
#include <random>
#include <chrono>

class RobotCommunicationSim : public RobotCommunication {
private:
    bool simulation_mode;
    std::array<float, 6> current_angles;
    std::mt19937 rng;
    std::uniform_real_distribution<float> angle_dist;
    
public:
    RobotCommunicationSim() 
        : simulation_mode(false),
          rng(std::chrono::system_clock::now().time_since_epoch().count()),
          angle_dist(-360.0f, 360.0f) {
        
        // Initialize to home position
        current_angles = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    }

    bool connect(const std::string& address) override {
        std::cout << "Connected to simulation mode at: " << address << std::endl;
        simulation_mode = true;
        return true;
    }

    void sendJointCommand(const std::array<float, 6>& angles) override {
        if (simulation_mode) {
            // Simulate sending command to robot
            std::cout << "Sending joint command: ";
            for (float angle : angles) std::cout << angle << " ";
            std::cout << std::endl;
            
            // Update local state (simulation)
            current_angles = angles;
        }
    }

    std::array<float, 6> readJointPositions() override {
        if (simulation_mode) {
            // Simulate reading from robot
            return current_angles;
        }
        return {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    }

    bool isConnected() const override {
        return simulation_mode;
    }

    void setSimulationMode(bool enabled) override {
        simulation_mode = enabled;
    }

    bool isSimulationMode() const override {
        return simulation_mode;
    }
};