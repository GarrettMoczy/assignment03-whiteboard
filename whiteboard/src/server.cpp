#include "server.h"

void Init(GLFWwindow* window) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        exit(1);
    }
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Whiteboard Server", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW.\n";
        exit(1);
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


void Bind(GLFWwindow* window, WhiteBoard& whiteboard) {
    glfwSetWindowUserPointer(window, &whiteboard);
    glfwSetMouseButtonCallback(window, WhiteBoard::StaticMouseCallback);
    glfwSetCursorPosCallback(window, WhiteBoard::StaticCursorPositionCallback);
    glfwSetCharCallback(window, WhiteBoard::StaticCharacterCallback);
    whiteboard.SetFrameBuffer();
    whiteboard.ClearMaskData();
}

server::server(float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3],
    float drawnBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3],
    bool mask[WINDOW_HEIGHT][WINDOW_WIDTH],
    GLFWwindow* window)
    : whiteboard(frameBuffer, drawnBuffer, mask, window) {
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed. Error: " << WSAGetLastError() << "\n";
        exit(1);
    }

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error: " << WSAGetLastError() << "\n";
        WSACleanup();
        exit(1);
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        exit(1);
    }

    std::cout << "Server started on port 8080.\n";
}

void server::Start() {
    GLFWwindow* tempWindow = whiteboard.GetWindow();
    Init(tempWindow);
    whiteboard.SetWindow(tempWindow);
    Bind(whiteboard.GetWindow(), whiteboard); 

    std::thread renderThread([this]() {
        while (!glfwWindowShouldClose(whiteboard.GetWindow())) {
            glClear(GL_COLOR_BUFFER_BIT);
            whiteboard.Display();
            glFlush();
            glfwSwapBuffers(whiteboard.GetWindow());
            glfwPollEvents();
        }
        });

    handleIncomingPackets();
    renderThread.join();
}


void server::sendPacket(uint8_t type, const std::vector<char>& payload, const sockaddr_in& client) {
    std::vector<char> packet;
    packet.push_back(static_cast<char>(type));
    packet.insert(packet.end(), payload.begin(), payload.end()); 

    int result = sendto(sock, packet.data(), packet.size(), 0, (sockaddr*)&client, sizeof(client));
    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to send packet to " << inet_ntoa(client.sin_addr)
            << ". Error: " << WSAGetLastError() << "\n";
    }
    else {
        std::cout << "Packet sent to " << inet_ntoa(client.sin_addr) << "\n";
    }
}


server::~server() {
    closesocket(sock);
    WSACleanup();
}

void server::addClient(sockaddr_in ip) {
    //check existence
    auto it = std::find_if(clientIPs.begin(), clientIPs.end(),
        [&ip](const sockaddr_in& addr) {
            return addr.sin_addr.s_addr == ip.sin_addr.s_addr && addr.sin_port == ip.sin_port;
        });

    if (it != clientIPs.end()) {
        std::cout << "Client is already connected: " << inet_ntoa(ip.sin_addr) << "\n";
        return;
    }

    clientIPs.push_back(ip);
    std::cout << "New client added: " << inet_ntoa(ip.sin_addr) << "\n";

    broadcastWhiteboardState();
}

void server::handleDisconnect(sockaddr_in ip) {
    auto it = std::find_if(clientIPs.begin(), clientIPs.end(),
        [&ip](const sockaddr_in& addr) {
            return addr.sin_addr.s_addr == ip.sin_addr.s_addr && addr.sin_port == ip.sin_port;
        });

    if (it != clientIPs.end()) {
        clientIPs.erase(it);
        std::cout << "Client disconnected: " << inet_ntoa(ip.sin_addr) << "\n";
        updateClients();
    }
    else {
        std::cerr << "Client not found in list for disconnect.\n";
    }
}


void server::broadcastWhiteboardState() {
    std::vector<char> serializedState;
    //get
    auto mask = whiteboard.GetMask();
    auto drawnBuffer = whiteboard.GetDrawnBuffer();

    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            serializedState.push_back(mask[y][x] ? 1 : 0);
            if (mask[y][x]) {
                serializedState.push_back(static_cast<char>(drawnBuffer[y][x][0] * 255));
                serializedState.push_back(static_cast<char>(drawnBuffer[y][x][1] * 255));
                serializedState.push_back(static_cast<char>(drawnBuffer[y][x][2] * 255));
            }
        }
    }

    for (const auto& client : clientIPs) {
        sendPacket(0x06, serializedState, client);
    }
}



void server::handleIncomingPackets() {
    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    while (true) {
        int bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            int errorCode = WSAGetLastError();
            if (errorCode == WSAETIMEDOUT) {
                std::cout << "Socket timeout while waiting for packets.\n";
                continue;
            }
            else {
                std::cerr << "Error receiving data. Code: " << errorCode << "\n";
                break;
            }
        }

        uint8_t packetType = buffer[0]; 
        std::vector<char> payload(buffer + 1, buffer + bytesReceived);

        //TODO
        switch (packetType) {
        case 0x01:
            addClient(clientAddr);
            break;
        case 0x05: 
            //TODO
            break;
        default:
            std::cerr << "Unknown packet type received: " << (int)packetType << "\n";
            break;
        }
    }
}

void server::endSession() {
    for (const auto& client : clientIPs) {
        sendPacket(0x03, {}, client); 
    }
    clientIPs.clear();
    std::cout << "Session ended. All clients notified.\n";
}

void server::updateClients() {
    std::vector<char> clientList;
    for (const auto& ip : clientIPs) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip.sin_addr, ipStr, INET_ADDRSTRLEN);
        clientList.insert(clientList.end(), ipStr, ipStr + strlen(ipStr) + 1);
    }

    for (const auto& client : clientIPs) {
        sendPacket(0x04, clientList, client);
    }

    std::cout << "Client list updated and broadcasted.\n";
}
