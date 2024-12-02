#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600



class Menu {

    struct color { float r, g, b; };

private:
    std::string img;
    GLFWwindow* window;
    float(*frameBuffer)[WINDOW_WIDTH][3];
    void SetFrameBufferPixel(int x, int y, struct color lc);

public:
    Menu(GLFWwindow* window, float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], const std::string& imagePath);
    void Display();

};