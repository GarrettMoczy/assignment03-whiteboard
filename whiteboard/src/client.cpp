#include "client.h"
#include <thread>

client::client() {

}
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

    sendPacket((uint8_t)1, {}, serverAddr); //initial connection packet
}

client::~client() {
    running = false;
    closesocket(sock);
    WSACleanup();
}

void client::receive() {
    while (running) {
        std::fill(readBuff.begin(), readBuff.end(), 0); // Clear the buffer
        sockaddr_in from;
        int fromSize = sizeof(from);
        int bytesReceived = recvfrom(sock, readBuff.data(), readBuff.size(), 0, (sockaddr*)&from, &fromSize);
        if (bytesReceived > 0) {
            uint8_t type = readBuff[0];
            readBuff.erase(readBuff.begin());
            handlePacket(type);
        }
    }
}

void client::DrawSquare(int xpos, int ypos, int xend, int yend, int size, struct color lc) {
    WhiteBoard::DrawSquare(xpos, ypos, xend, yend, size, lc);
    struct drawArgs args;
    args.xpos = xpos;
    args.ypos = ypos;
    args.xend = xend;
    args.yend = yend;
    args.size = size;
    args.lc.r = lc.r;
    args.lc.g = lc.g;
    args.lc.b = lc.b;
    std::vector<char> packet;
    packet.resize(sizeof(struct drawArgs));
    std::memcpy(packet.data(), &args, sizeof(struct drawArgs));
    for (auto ip : clientIPs) {
        sendPacket((uint8_t)5, packet, ip);
    }
    
}


void client::handlePacket(uint8_t type) {
    switch (type) {
    case 0x03: // Session End
        running = false; // Stop threads
        break;

    case 0x04: { // Update client list
        clientIPs.clear();
        const char* data = readBuff.data();  // Enclose in a block
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
        struct drawArgs args;
        std::memcpy(&args, readBuff.data(), readBuff.size());
        WhiteBoard::DrawSquare(args.xpos, args.ypos, args.xend, args.yend, args.size, args.lc);
        break;

    default:
        std::cerr << "Unhandled packet type: " << static_cast<int>(type) << "\n";
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
