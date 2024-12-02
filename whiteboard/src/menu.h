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
    std::string password;
public:
    Menu(GLFWwindow* window, float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], const std::string& imagePath);
    void Display();
    void CharacterCallback(GLFWwindow* lWindow, unsigned int key);
    void KeyCallback(GLFWwindow* lWindow, int key, int scancode, int action, int mods);
    void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos);
    static void StaticCharacterCallback(GLFWwindow* lWindow, unsigned int key);
    static void StaticKeyCallback(GLFWwindow* lWindow, int key, int scancode, int action, int mods);
    static void StaticCursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos);
    bool hostPress(int xpos, int ypos);
    bool joinPress(int xpos, int ypos);
    bool isActive();
    void setInactive();
    bool active;
};