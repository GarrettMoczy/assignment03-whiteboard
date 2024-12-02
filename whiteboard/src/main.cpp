#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include "whiteboard.h"
#include "menu.h"
#include "server.h"
#include "client.h"

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
float drawnBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
bool mask[WINDOW_HEIGHT][WINDOW_WIDTH];
GLFWwindow* window;

void Init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        exit(1);
    }
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Whiteboard App", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW.\n";
        glfwTerminate();
        exit(1);
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Bind(WhiteBoard& whiteboard) {
    glfwSetWindowUserPointer(window, &whiteboard);
    glfwSetMouseButtonCallback(window, WhiteBoard::StaticMouseCallback);
    glfwSetCursorPosCallback(window, WhiteBoard::StaticCursorPositionCallback);
    glfwSetCharCallback(window, WhiteBoard::StaticCharacterCallback);
    whiteboard.SetFrameBuffer();
    whiteboard.ClearMaskData();
}
void Bind(Menu menu) {
    glfwSetWindowUserPointer(window, &menu);
    glfwSetCharCallback(window, Menu::StaticCharacterCallback);
    glfwSetKeyCallback(window, Menu::StaticKeyCallback);
    glfwSetCursorPosCallback(window, Menu::StaticCursorPositionCallback);
}

int main() {
    server* whiteboard;
    Menu menu(window, frameBuffer, "../img/alt_menu_texture.png");

    Init();
    Bind(menu);
    while (glfwWindowShouldClose(window) == 0 && menu.isActive())
    {
        glfwSetWindowUserPointer(window, &menu);
        menu.Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    //if (menu.server) {
        whiteboard = new server(frameBuffer, drawnBuffer, mask, window);
    //} else {
    //    whiteboard = new client(menu.password, frameBuffer, drawnBuffer, mask, window);
    //}

    Bind(*whiteboard);
    std::thread receiveThread(&client::receive, whiteboard);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        whiteboard->Display();
        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    receiveThread.join();
    glfwTerminate();
    return 0;
}