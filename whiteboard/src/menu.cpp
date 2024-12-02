#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Include the stb_image library
#include "Menu.h"
#include <iostream>

Menu::Menu(GLFWwindow* window, float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], const std::string& imagePath) {
    img = imagePath;
    this->frameBuffer = frameBuffer;
    this->window = window;
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

