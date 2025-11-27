    #pragma once
    #include <string>
    #include <array>
    #include <vector>
    #include <mutex>
    #include <thread>
    #include <fstream>
    #include <functional>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <fcntl.h>

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
        int serverSocket;
        int clientSocket;
        
        std::thread commThread;
        std::mutex dataMutex;
        
        std::array<float, 6> currentJoints;
        std::array<float, 6> targetJoints;

        void CommunicationLoop();
        void SetupServer();
        void HandleClient();
        std::vector<std::string> SplitString(const std::string& str, char delimiter);
    };