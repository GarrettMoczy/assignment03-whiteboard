#include "client.h"
#include <thread>

client::client(std::string serverIP) : running(true), readBuff(1024, 0) {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed. Error: " << WSAGetLastError() << std::endl;
        running = false;
        return;
    }

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket initialization failed. Error: " << WSAGetLastError() << std::endl;
        running = false;
        return;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid server IP address: " << serverIP << std::endl;
        running = false;
    }
}

client::~client() {
    running = false;
    closesocket(sock);
    WSACleanup();
}

void client::send() {
    while (running) {
        //initial connection packet
        sendPacket((uint8_t)1, {}, serverAddr);
        

        if (input == "q") {
            running = false;
            sendPacket(0x03, {}, serverAddr); // Disconnect packet
        }
        else if (input == "whiteboard") {
            std::vector<char> payload{ 'W', 'B', 'U', 'P', 'D' }; // Example payload
            sendPacket(0x05, payload, serverAddr);
        }
    }
}

void client::receive() {
    while (running) {
        std::fill(readBuff.begin(), readBuff.end(), 0); // Clear the buffer
        sockaddr_in from;
        int fromSize = sizeof(from);
        int bytesReceived = recvfrom(sock, readBuff.data(), readBuff.size(), 0, (sockaddr*)&from, &fromSize);
        if (bytesReceived > 0) {
            uint8_t type = readBuff[0];
            handlePacket(type);
        }
    }
}

void client::handlePacket(uint8_t type) {
    switch (type) {
    case 0x03: // Disconnect
        running = false; // Stop threads
        break;
    case 0x04: { // Update client list
        clientIPs.clear();
        const char* data = readBuff.data() + 1; // Skip packet type
        while (*data) {
            sockaddr_in addr = {};
            if (inet_pton(AF_INET, data, &addr.sin_addr) > 0) {
                addr.sin_family = AF_INET;
                addr.sin_port = htons(8080);
                clientIPs.push_back(addr);
            }
            data += strlen(data) + 1; // Move to the next string
        }
        break;
    }
    case 0x05: // Update whiteboard
        // To be implemented
        break;
    }
}

void client::sendPacket(unsigned int type, const std::vector<char>& payload, const sockaddr_in& recipient) {
    std::vector<char> packet;
    packet.push_back(static_cast<char>(type));
    packet.insert(packet.end(), payload.begin(), payload.end());

    if (sendto(sock, packet.data(), packet.size(), 0, (sockaddr*)&recipient, sizeof(recipient)) == SOCKET_ERROR) {
        std::cerr << "Failed to send packet to " << inet_ntoa(recipient.sin_addr) << ". Error: " << WSAGetLastError() << std::endl;
    }
}


//int main() {
//	client c(127.0.0.1);
//	std::thread sendThread(&client::send, &c);
//	std::thread receiveThread(&client::receive, &c);
//
//	sendThread.join();
//	receiveThread.join();
//
//}