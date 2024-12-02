#include "server.h"


server::server() {
	//initiate winsock, error handling
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Winsock initialization failed. Error: " << WSAGetLastError() << std::endl;
		exit(1);
	}
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		std::cerr << "Socket initialization failed. Error: " << WSAGetLastError() << std::endl;
	}

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);

	if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Bind failed. Error: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	std::cout << "Server started" << std::endl;
}

server::~server() {

	closesocket(sock);
	WSACleanup();
}

void server::updateClients() {
	//send updated client list to all clients
	std::vector<char> clientList;
	for (auto ip : clientIPs) {

		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &ip.sin_addr, ipStr, INET_ADDRSTRLEN);
		clientList.insert(clientList.end(), ipStr, ipStr + INET_ADDRSTRLEN);
		clientList.push_back('\0');
	}

	for (auto ip : clientIPs) {
		sendPacket(0x04, clientList, ip);
	}
}

void server::addClient(sockaddr_in ip) {
	if (std::find(clientIPs.begin(), clientIPs.end(), ip) != clientIPs.end()) {
		std::cerr << "Client is already connected" << std::endl;
		return;
	}

	clientIPs.push_back(ip);

	//send current whiteboard to client
	std::vector<char> mask;
	//buildWhiteboardMask(mask);
	sendPacket(0x06, mask, ip);

	updateClients();


}


//TODO

//this will need to be fixed but just place holder for now
//void server::buildWhiteboardMask(std::vector<char>& mask) {
//	//build the whiteboard mask to send to clients
//	for (int y = 0; y < WINDOW_HEIGHT; y++) {
//		for (int x = 0; x < WINDOW_WIDTH; x++) {
//			if (mask[y][x]) {
//				//serialize?
//			}
//		}
//	}
//}

void server::handleDisconnect(sockaddr_in ip) {
	auto it = std::find(clientIPs.begin(), clientIPs.end(), ip);
	if (it != clientIPs.end()) {
		clientIPs.erase(it);
		updateClients();
	}
	else {
		std::cerr << "IP not found in client list" << std::endl;
	}


}

void server::endSession() {
	//send disconnect packet to all
	for (auto ip : clientIPs) {
		sendPacket(0x03, {}, ip);
	}

	clientIPs.clear();
}

//int main() {
//	server myServer;
//	char buffer[1024];
//	sockaddr_in clientAddr;
//	int clientAddrSize = sizeof(clientAddr);
//
//	while (true) {
//		int bytesReceived = recvfrom(myServer.sock, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientAddrSize);
//		if (bytesReceived == SOCKET_ERROR) {
//			std::cerr << "Error receiving packet." << WSAGetLastError() << std::endl;
//			continue;
//		}
//
//
//		uint8_t packetType = buffer[0]; //assuming first byte is packet type
//		std::vector<char> payload(buffer + 1, buffer + bytesReceived);
//
//		switch (packetType) {
//		case 0x01:
//			myServer.addClient(clientAddr);
//			break;
//		case 0x02:
//			myServer.handleDisconnect(clientAddr);
//			break;
//		case 0x05:
//			//do whiteboard update
//			break;
//		default:
//			std::cerr << "Unknown packet received: " << (int)packetType << std::endl;
//			break;
//		}
//	}
//}

