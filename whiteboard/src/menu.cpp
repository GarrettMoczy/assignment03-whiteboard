#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Include the stb_image library
#include "Menu.h"
#include <iostream>

Menu::Menu(GLFWwindow* window, float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], const std::string& imagePath) {
    img = imagePath;
    this->frameBuffer = frameBuffer;
    this->window = window;
    active = true;
}

void Menu::SetFrameBufferPixel(int x, int y, struct color lc)
{
    y = WINDOW_HEIGHT - 1 - y;
    x = CLAMP(x, 0, WINDOW_WIDTH - 1);
    y = CLAMP(y, 0, WINDOW_HEIGHT - 1);
    frameBuffer[y][x][0] = lc.r;
    frameBuffer[y][x][1] = lc.g;
    frameBuffer[y][x][2] = lc.b;
};
void Menu::setInactive() {
    active = false;
}
bool Menu::hostPress(int xpos, int ypos) {
    return (xpos <= 290 && xpos >= 250) && (ypos <= 540 && ypos >= 350);

};
bool Menu::joinPress(int xpos, int ypos) {
    return (xpos <= 200 && xpos >= 160) && (ypos <= 540 && ypos >= 350);
};
void Menu::Display() {

    glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);

    int imgWidth, imgHeight, channels;
    unsigned char* data = stbi_load(img.c_str(), &imgWidth, &imgHeight, &channels, 3);
    if (data) {
        for (int y = 0; y < WINDOW_HEIGHT; ++y) {
            for (int x = 0; x < WINDOW_WIDTH; ++x) {
                int index = (y * imgWidth + x) * 3; // 3 channels per pixel (RGB)

                 // Set the pixel in the framebuffer using the data array values
                float red = data[index] / 255.0f;
                float green = data[index + 1] / 255.0f;
                float blue = data[index + 2] / 255.0f;

                SetFrameBufferPixel(x, y, {red, green, blue});
            }
        }
    }
    else {
        std::cerr << "Failed to load texture: " << img << std::endl;
    }

    stbi_image_free(data);

}
void Menu::CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
    system("cls");  // Clear the console

    // Check if the key is a number, dot, or semicolon
    if ((key >= '0' && key <= '9') || key == '.' || key == ':') {
        // Append the character to the password string
        password.push_back(static_cast<char>(key));
    }
    // Check if the key is backspace
    else if (key == GLFW_KEY_BACKSPACE) {
        // Remove the last character from the password string, if it exists
        if (!password.empty()) {
            password.pop_back();
        }
    }

    // Print the current password
    printf("IP: %s\n", password.c_str());
}

void Menu::KeyCallback(GLFWwindow* lWindow, int key, int scancode, int action, int mods)
{

    // Handle the Backspace key (GLFW_KEY_BACKSPACE)
    if (action == GLFW_PRESS && key == GLFW_KEY_BACKSPACE) {
        system("cls");  // Clear the console
        // Remove the last character from the password string, if it exists
        if (!password.empty()) {
            password.pop_back();
        }

        // Print the updated password
        printf("IP: %s\n", password.c_str());
    }
}

void Menu::CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
    int framebufferX = WINDOW_HEIGHT - 1 - (int)ypos;  // Invert the new y-coordinate
    int framebufferY = (int)xpos;                      // Swap x and y

    int state = glfwGetMouseButton(lWindow, GLFW_MOUSE_BUTTON_LEFT);

    static bool isMousePressed = false;
    static int pressX, pressY;

    if (state == GLFW_PRESS) {
        if (!isMousePressed) {
            // Store the position where the mouse was pressed
            pressX = framebufferX;
            pressY = framebufferY;
            isMousePressed = true;
        }
    }
    else if (state == GLFW_RELEASE) {
        if (isMousePressed) {
            // Check if the mouse was released at the same position
            if (hostPress(framebufferX, framebufferY) && hostPress(pressX, pressY)) {
                // Call the function you want to trigger
                printf("pressed: host\n");
                this->active = false;
            }
            else if (joinPress(framebufferX, framebufferY) && joinPress(pressX, pressY)) {
                
                printf("pressed: join\n");
                this->active = false;
            }
            isMousePressed = false;
            printf("Active state updated: %d\n", active);
        }
    }
}




// Static wrapper for CursorPositionCallback
void Menu::StaticCursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos) {
    Menu* menu = static_cast<Menu*>(glfwGetWindowUserPointer(lWindow));
    if (menu) {
        menu->CursorPositionCallback(lWindow, xpos, ypos);
    }
}


// Static wrapper for KeyPositionCallback
void Menu::StaticKeyCallback(GLFWwindow* lWindow, int key, int scancode, int action, int mods) {
    Menu* menu = static_cast<Menu*>(glfwGetWindowUserPointer(lWindow));
    if (menu) {
        menu->KeyCallback(lWindow, key, scancode, action, mods);
    }
}

// Static wrapper for CharacterCallback
void Menu::StaticCharacterCallback(GLFWwindow* lWindow, unsigned int key) {
    Menu* menu = static_cast<Menu*>(glfwGetWindowUserPointer(lWindow));
    if (menu) {
        menu->CharacterCallback(lWindow, key);
    }
}

bool Menu::isActive() {
    return active;
}
