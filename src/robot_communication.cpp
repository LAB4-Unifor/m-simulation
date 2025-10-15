#include "robot_communication.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>

RobotCommunication::RobotCommunication() 
    : connectionState(ConnectionState::DISCONNECTED),
      simulationMode(SimulationMode::MANUAL),
      running(false),
      loggingEnabled(false),
      serverPort(502),
      updateRate(50) {
    
    currentJointAngles.fill(0.0f);
    targetJointAngles.fill(0.0f);
}

RobotCommunication::~RobotCommunication() {
    disconnect();
    if (loggingEnabled) {
        stopLogging();
    }
}

bool RobotCommunication::connect(const std::string& address, int port) {
    if (isConnected()) {
        disconnect();
    }
    
    serverAddress = address;
    serverPort = port;
    connectionState = ConnectionState::CONNECTING;
    
    if (connectionCallback) {
        connectionCallback(connectionState);
    }
    
    running = true;
    commThread = std::thread(&RobotCommunication::communicationThread, this);
    
    return true;
}

void RobotCommunication::disconnect() {
    running = false;
    if (commThread.joinable()) {
        commThread.join();
    }
    connectionState = ConnectionState::DISCONNECTED;
    
    if (connectionCallback) {
        connectionCallback(connectionState);
    }
}

bool RobotCommunication::isConnected() const {
    return connectionState == ConnectionState::CONNECTED;
}

RobotCommunication::ConnectionState RobotCommunication::getConnectionState() const {
    return connectionState;
}

void RobotCommunication::setSimulationMode(SimulationMode mode) {
    simulationMode = mode;
}

RobotCommunication::SimulationMode RobotCommunication::getSimulationMode() const {
    return simulationMode;
}

bool RobotCommunication::sendJointAngles(const std::array<float, 6>& angles) {
    std::lock_guard<std::mutex> lock(dataMutex);
    targetJointAngles = angles;
    return true;
}

std::array<float, 6> RobotCommunication::readJointAngles() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return currentJointAngles;
}

bool RobotCommunication::sendCommand(const std::string& command) {
    std::cout << "Sending command: " << command << std::endl;
    return true;
}

std::string RobotCommunication::readResponse() {
    return "OK";
}

void RobotCommunication::setConnectionCallback(std::function<void(ConnectionState)> callback) {
    connectionCallback = callback;
}

void RobotCommunication::setDataReceivedCallback(std::function<void(const std::array<float, 6>&)> callback) {
    dataReceivedCallback = callback;
}

void RobotCommunication::setErrorCallback(std::function<void(const std::string&)> callback) {
    errorCallback = callback;
}

void RobotCommunication::startLogging(const std::string& filename) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    logFile.open(filename, std::ios::out);
    if (logFile.is_open()) {
        logFilename = filename;
        loggingEnabled = true;
        logFile << "timestamp,j1,j2,j3,j4,j5,j6,mode\n";
    } else {
        if (errorCallback) {
            errorCallback("Failed to open log file: " + filename);
        }
    }
}

void RobotCommunication::stopLogging() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (logFile.is_open()) {
        logFile.close();
    }
    loggingEnabled = false;
}

bool RobotCommunication::isLogging() const {
    return loggingEnabled;
}

bool RobotCommunication::exportTrajectory(const std::string& filename, const std::vector<std::array<float, 6>>& trajectory) {
    std::ofstream file(filename, std::ios::out);
    if (!file.is_open()) {
        return false;
    }
    
    file << "j1,j2,j3,j4,j5,j6\n";
    
    for (const auto& angles : trajectory) {
        for (size_t i = 0; i < angles.size(); i++) {
            file << angles[i];
            if (i < angles.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

bool RobotCommunication::importTrajectory(const std::string& filename, std::vector<std::array<float, 6>>& trajectory) {
    std::ifstream file(filename, std::ios::in);
    if (!file.is_open()) {
        return false;
    }
    
    trajectory.clear();
    std::string line;
    
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        std::array<float, 6> angles;
        size_t pos = 0;
        size_t lastPos = 0;
        int index = 0;
        
        while ((pos = line.find(',', lastPos)) != std::string::npos && index < 6) {
            angles[index++] = std::stof(line.substr(lastPos, pos - lastPos));
            lastPos = pos + 1;
        }
        
        if (index < 6) {
            angles[index] = std::stof(line.substr(lastPos));
        }
        
        trajectory.push_back(angles);
    }
    
    file.close();
    return true;
}

void RobotCommunication::communicationThread() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    if (establishConnection()) {
        connectionState = ConnectionState::CONNECTED;
        if (connectionCallback) {
            connectionCallback(connectionState);
        }
    } else {
        connectionState = ConnectionState::ERROR;
        if (connectionCallback) {
            connectionCallback(connectionState);
        }
        if (errorCallback) {
            errorCallback("Failed to establish connection");
        }
        return;
    }
    
    auto lastUpdate = std::chrono::steady_clock::now();
    
    while (running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);
        
        if (elapsed.count() >= 1000 / updateRate) {
            processIncomingData();
            lastUpdate = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    connectionState = ConnectionState::DISCONNECTED;
    if (connectionCallback) {
        connectionCallback(connectionState);
    }
}

bool RobotCommunication::establishConnection() {
    std::cout << "Establishing connection to " << serverAddress << ":" << serverPort << std::endl;
    return true;
}

void RobotCommunication::processIncomingData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    for (int i = 0; i < 6; i++) {
        float diff = targetJointAngles[i] - currentJointAngles[i];
        float maxChange = 0.5f;
        
        if (std::abs(diff) > maxChange) {
            currentJointAngles[i] += (diff > 0 ? maxChange : -maxChange);
        } else {
            currentJointAngles[i] = targetJointAngles[i];
        }
    }
    
    if (loggingEnabled && logFile.is_open()) {
        logData(currentJointAngles);
    }
    
    if (dataReceivedCallback) {
        dataReceivedCallback(currentJointAngles);
    }
}

void RobotCommunication::logData(const std::array<float, 6>& angles) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    logFile << timestamp << ",";
    for (size_t i = 0; i < angles.size(); i++) {
        logFile << angles[i];
        if (i < angles.size() - 1) {
            logFile << ",";
        }
    }
    
    switch (simulationMode) {
        case SimulationMode::MANUAL: logFile << ",MANUAL"; break;
        case SimulationMode::PLAYBACK: logFile << ",PLAYBACK"; break;
        case SimulationMode::REAL_TIME_SYNC: logFile << ",REAL_TIME_SYNC"; break;
        case SimulationMode::SIMULATION_ONLY: logFile << ",SIMULATION_ONLY"; break;
    }
    
    logFile << "\n";
}
