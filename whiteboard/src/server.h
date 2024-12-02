#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>

#include "client.h"

#pragma comment(lib, "ws2_32.lib") // Automatically link the Winsock library

/*
* Intended functionality:
* server sits in infinite loop receiving packets
* case 1: packet is a new client:
* add client IP to vector, send current whiteboard state to new client
* update clients by sending client list
* case 2: packet indicates client disconnect:
* remove client IP from vector, update clients by sending client list
* case 3: whiteboard update packet:
* make the necessary changes to the whiteboard
* this ensures the whiteboard is up to date for new clients joining the session
*/

//custom equality operator for sockaddr_in
inline bool operator==(const sockaddr_in& a, const sockaddr_in& b) {
    return a.sin_family == b.sin_family &&
        a.sin_port == b.sin_port &&
        a.sin_addr.s_addr == b.sin_addr.s_addr;
}
class server : public client{
public:
    server();
    ~server();
    void addClient(sockaddr_in ip);
    void handleDisconnect(sockaddr_in ip);
    void endSession();
    SOCKET sock;


private:
    //std::string inviteCode;
    WSADATA wsaData;

    std::vector<sockaddr_in> clientIPs; //list of each clients IP

    //void createInviteCode();
    void updateClients(); //sends a copy of the clientIP vector
    //void sendPacket(unsigned int type, const std::vector<char>& payload, const sockaddr_in& client);
    //void buildWhiteboardMask(std::vector<char>& mask);
};