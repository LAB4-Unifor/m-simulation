#include "robot_communication.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <sstream>
#include <vector>

// Linux headers para Sockets
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

RobotCommunication::RobotCommunication() 
    : state(ConnectionState::DISCONNECTED), running(false), serverPort(10001), serverSocket(-1), clientSocket(-1) {
    currentJoints.fill(0.0f);
    targetJoints.fill(0.0f);
}

RobotCommunication::~RobotCommunication() {
    Disconnect();
}

void RobotCommunication::Connect(const std::string& ip, int port) {
    if (running) Disconnect();
    
    serverPort = port;
    running = true;
    state = ConnectionState::CONNECTING;
    
    // Inicia a thread que vai gerenciar o servidor
    commThread = std::thread(&RobotCommunication::CommunicationLoop, this);
}

void RobotCommunication::Disconnect() {
    running = false;
    if (clientSocket != -1) close(clientSocket);
    if (serverSocket != -1) close(serverSocket);
    
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
    // Aqui atualizaríamos o estado interno para refletir o que a simulação está fazendo
    currentJoints = angles;
}

// Utilitário para quebrar string
std::vector<std::string> RobotCommunication::SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void RobotCommunication::SetupServer() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "COMM: Failed to create socket" << std::endl;
        state = ConnectionState::ERROR_STATE;
        return;
    }

    // Permite reusar a porta imediatamente após fechar
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "COMM: Bind failed on port " << serverPort << std::endl;
        state = ConnectionState::ERROR_STATE;
        return;
    }

    listen(serverSocket, 1);
    std::cout << "COMM: Server listening on port " << serverPort << "..." << std::endl;
}

void RobotCommunication::HandleClient() {
    char buffer[1024];
    // Configura socket como Non-Blocking para não travar a thread
    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

    int bytesRead = recv(clientSocket, buffer, 1024, 0);
    
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string command(buffer);
        
        // Remove quebra de linha
        if (!command.empty() && command.back() == '\n') command.pop_back();
        
        std::cout << "COMM: Received command: " << command << std::endl;

        auto tokens = SplitString(command, ' ');
        if (tokens.empty()) return;

        if (tokens[0] == "MOVE" && tokens.size() >= 7) {
            // PROTOCOLO: MOVE J1 J2 J3 J4 J5 J6
            std::lock_guard<std::mutex> lock(dataMutex);
            for(int i=0; i<6; i++) {
                try {
                    targetJoints[i] = std::stof(tokens[i+1]);
                } catch(...) {}
            }
            std::string reply = "OK MOVING\n";
            send(clientSocket, reply.c_str(), reply.length(), 0);
        }
        else if (tokens[0] == "GETPOS") {
            // PROTOCOLO: GETPOS -> Retorna ângulos atuais
            std::stringstream ss;
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                ss << "POS " << currentJoints[0] << " " << currentJoints[1] << " " 
                   << currentJoints[2] << " " << currentJoints[3] << " " 
                   << currentJoints[4] << " " << currentJoints[5] << "\n";
            }
            std::string reply = ss.str();
            send(clientSocket, reply.c_str(), reply.length(), 0);
        }
    } else if (bytesRead == 0) {
        // Cliente desconectou
        close(clientSocket);
        clientSocket = -1;
        state = ConnectionState::DISCONNECTED;
        std::cout << "COMM: Client disconnected." << std::endl;
    }
}

void RobotCommunication::CommunicationLoop() {
    SetupServer();

    while (running) {
        if (serverSocket == -1) break;

        if (clientSocket == -1) {
            // Aceitar conexão
            sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            
            // Aceita conexão (Blocking neste ponto, mas temos timeout ou thread separada)
            // Para simplicidade, usamos accept bloqueante mas verificamos 'running'
            fd_set set;
            struct timeval timeout;
            FD_ZERO(&set);
            FD_SET(serverSocket, &set);
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000; // 100ms timeout

            int rv = select(serverSocket + 1, &set, NULL, NULL, &timeout);
            if(rv > 0) {
                clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
                if (clientSocket >= 0) {
                    state = ConnectionState::CONNECTED;
                    std::cout << "COMM: Client Connected!" << std::endl;
                }
            }
        } else {
            // Processar dados do cliente conectado
            HandleClient();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    if (serverSocket != -1) close(serverSocket);
}