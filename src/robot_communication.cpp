#include "robot_communication.h"
#include <iostream>
#include <chrono>

RobotCommunication::RobotCommunication() 
    : state(ConnectionState::DISCONNECTED), running(false), serverPort(502) {
    currentJoints.fill(0.0f);
    targetJoints.fill(0.0f);
}

RobotCommunication::~RobotCommunication() {
    Disconnect();
}

void RobotCommunication::Connect(const std::string& ip, int port) {
    if (running) Disconnect();
    
    serverIP = ip;
    serverPort = port;
    running = true;
    state = ConnectionState::CONNECTING;
    
    commThread = std::thread(&RobotCommunication::CommunicationLoop, this);
}

void RobotCommunication::Disconnect() {
    running = false;
    if (commThread.joinable()) {
        commThread.join();
    }
    state = ConnectionState::DISCONNECTED;
}

bool RobotCommunication::IsConnected() const {
    return state == ConnectionState::CONNECTED;
}

std::array<float, 6> RobotCommunication::ReadJointAngles() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return currentJoints;
}

void RobotCommunication::SendJointAngles(const std::array<float, 6>& angles) {
    std::lock_guard<std::mutex> lock(dataMutex);
    targetJoints = angles;
}

void RobotCommunication::CommunicationLoop() {
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    state = ConnectionState::CONNECTED;
    std::cout << "COMM: Connected to " << serverIP << ":" << serverPort << std::endl;

    while (running) {
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            // In a real scenario, this would read/write to a TCP socket
            // For simulation, we just sync current to target with smoothing
            for(int i=0; i<6; i++) {
                float diff = targetJoints[i] - currentJoints[i];
                if (std::abs(diff) > 0.1f) {
                    currentJoints[i] += diff * 0.1f; // Simple smoothing
                } else {
                    currentJoints[i] = targetJoints[i];
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // 50Hz
    }
    
    state = ConnectionState::DISCONNECTED;
    std::cout << "COMM: Disconnected." << std::endl;
}