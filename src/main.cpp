#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "whiteboard.h"

#include "menu.h"
#include "server.h"
#include "client.h"
=======

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
float drawnBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
bool mask[WINDOW_HEIGHT][WINDOW_WIDTH];
GLFWwindow* window;

void Init()
{
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 03 - OpenGL Whiteboard", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


}

void Bind(WhiteBoard whiteboard) {
    glfwSetWindowUserPointer(window, &whiteboard);
    glfwSetMouseButtonCallback(window, WhiteBoard::StaticMouseCallback);
    glfwSetCursorPosCallback(window, WhiteBoard::StaticCursorPositionCallback);
    glfwSetCharCallback(window, WhiteBoard::StaticCharacterCallback);
    whiteboard.SetFrameBuffer();
    whiteboard.ClearMaskData();
}

int main()
{
    WhiteBoard whiteboard(frameBuffer, drawnBuffer, mask, window);
    Menu menu(window, frameBuffer, "../img/alt_menu_texture.png");
    Init();

    while (true)
    {
        menu.Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    Bind(whiteboard);
    while (glfwWindowShouldClose(window) == 0) {
        glClear(GL_COLOR_BUFFER_BIT);
        whiteboard.Display();
        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
