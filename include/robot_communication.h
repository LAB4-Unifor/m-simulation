#ifndef ROBOT_COMMUNICATION_H
#define ROBOT_COMMUNICATION_H

#include <array>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <fstream>

class RobotCommunication {
public:
    enum class ConnectionState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        ERROR
    };
    
    enum class SimulationMode {
        MANUAL,
        PLAYBACK,
        REAL_TIME_SYNC,
        SIMULATION_ONLY
    };
    
    RobotCommunication();
    virtual ~RobotCommunication();
    
    virtual bool connect(const std::string& address, int port = 502);
    virtual void disconnect();
    virtual bool isConnected() const;
    ConnectionState getConnectionState() const;
    
    virtual bool sendJointAngles(const std::array<float, 6>& angles);
    virtual std::array<float, 6> readJointAngles();
    virtual bool sendCommand(const std::string& command);
    virtual std::string readResponse();
    
    void setSimulationMode(SimulationMode mode);
    SimulationMode getSimulationMode() const;
    
    void setConnectionCallback(std::function<void(ConnectionState)> callback);
    void setDataReceivedCallback(std::function<void(const std::array<float, 6>&)> callback);
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    void startLogging(const std::string& filename);
    void stopLogging();
    bool isLogging() const;
    
    bool exportTrajectory(const std::string& filename, const std::vector<std::array<float, 6>>& trajectory);
    bool importTrajectory(const std::string& filename, std::vector<std::array<float, 6>>& trajectory);
    
protected:
    virtual bool establishConnection();
    virtual void communicationThread();
    virtual void processIncomingData();
    virtual void logData(const std::array<float, 6>& angles);
    
    std::thread commThread;
    std::mutex dataMutex;
    std::atomic<ConnectionState> connectionState;
    std::atomic<SimulationMode> simulationMode;
    std::atomic<bool> running;
    
    std::array<float, 6> currentJointAngles;
    std::array<float, 6> targetJointAngles;
    
    std::function<void(ConnectionState)> connectionCallback;
    std::function<void(const std::array<float, 6>&)> dataReceivedCallback;
    std::function<void(const std::string&)> errorCallback;
    
    std::ofstream logFile;
    std::atomic<bool> loggingEnabled;
    std::string logFilename;
    
    std::string serverAddress;
    int serverPort;
    int updateRate;
};

#endif
