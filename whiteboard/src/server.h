#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <iostream>
#include <thread>
#include "whiteboard.h"

#pragma comment(lib, "ws2_32.lib")

class server : public WhiteBoard {
public:
    server();
    server(float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3],
        float drawnBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3],
        bool mask[WINDOW_HEIGHT][WINDOW_WIDTH],
        GLFWwindow* window);
    ~server();

    void Start();
    void addClient(sockaddr_in ip);
    void handleDisconnect(sockaddr_in ip);
    void broadcastWhiteboardState();
    void endSession();

private:
    SOCKET sock;
    WSADATA wsaData;
    std::vector<sockaddr_in> clientIPs;
    WhiteBoard whiteboard;

    void sendPacket(uint8_t type, const std::vector<char>& payload, const sockaddr_in& client);
    void handleIncomingPackets();
    void updateClients();
};
