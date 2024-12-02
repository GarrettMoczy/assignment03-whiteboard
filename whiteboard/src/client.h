#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

#include "whiteboard.h"

#pragma comment(lib, "ws2_32.lib") // Automatically link the Winsock library
#pragma pack(push, 1)
struct drawArgs {
	int xpos, ypos, xend, yend, size;
	struct color lc;
};
#pragma pack(pop)


//custom equality operator for sockaddr_in
inline bool operator==(const sockaddr_in& a, const sockaddr_in& b) {
	return a.sin_family == b.sin_family &&
		a.sin_port == b.sin_port &&
		a.sin_addr.s_addr == b.sin_addr.s_addr;
}


class client : virtual public WhiteBoard {

	public:
		client();
		client(std::string serverIP, float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], float drawnBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], bool mask[WINDOW_HEIGHT][WINDOW_WIDTH], GLFWwindow* window);
		~client();
		void receive();
		void handlePacket(uint8_t type);
		//void DrawSquare(int xpos, int ypos, int xend, int yend, int size, struct color lc);
		void DrawSquare(int xpos, int ypos, int xend, int yend, int size, struct color lc) override;

		//void updateWhiteboard();

	protected:
		
		WSADATA wsaData;
		SOCKET sock;

		sockaddr_in serverAddr;

		void sendPacket(unsigned int type, const std::vector<char>& payload, const sockaddr_in& recipient);
		
		bool running;
		std::vector<char> readBuff;
		std::vector<sockaddr_in> clientIPs; //list of each clients IP for transmission to clients
		//Whiteboard board; //instance of whiteboard sent to new members
};