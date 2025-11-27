    #pragma once
    #include <string>
    #include <array>
    #include <vector>
    #include <mutex>
    #include <thread>
    #include <fstream>
    #include <functional>

    class RobotCommunication {
    public:
        enum class ConnectionState { DISCONNECTED, CONNECTING, CONNECTED, ERROR_STATE };

        RobotCommunication();
        ~RobotCommunication();

        void Connect(const std::string& ip, int port);
        void Disconnect();
        bool IsConnected() const;
        
        // Data Access
        std::array<float, 6> ReadJointAngles();
        void SendJointAngles(const std::array<float, 6>& angles);

    private:
        std::string serverIP;
        int serverPort;
        ConnectionState state;
        bool running;
        
        std::thread commThread;
        std::mutex dataMutex;
        
        std::array<float, 6> currentJoints;
        std::array<float, 6> targetJoints;

        void CommunicationLoop();
    };