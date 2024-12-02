#include "whiteboard.h"


WhiteBoard::WhiteBoard(float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], float drawnBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], bool mask[WINDOW_HEIGHT][WINDOW_WIDTH],
    GLFWwindow* window) {
    brushSize = 4;
    brushColor = { 0.0f, 0.0f, 0.0f };
    backColor = { 1.0f, 1.0f, 1.0f };
    this->window = window;
    this->frameBuffer = frameBuffer;
    this->drawnBuffer = drawnBuffer;
    this->mask = mask;
    line = false;
    isMousePressed = false;
}

void WhiteBoard::SetFrameBufferPixel(int x, int y, struct color lc)
{
    y = WINDOW_HEIGHT - 1 - y;
    x = CLAMP(x, 0, WINDOW_WIDTH - 1);
    y = CLAMP(y, 0, WINDOW_HEIGHT - 1);
    frameBuffer[y][x][0] = lc.r;
    frameBuffer[y][x][1] = lc.g;
    frameBuffer[y][x][2] = lc.b;
};

void WhiteBoard::SetDrawnBufferPixel(int x, int y, struct color lc)
{
    y = WINDOW_HEIGHT - 1 - y;
    x = CLAMP(x, 0, WINDOW_WIDTH - 1);
    y = CLAMP(y, 0, WINDOW_HEIGHT - 1);
    drawnBuffer[y][x][0] = lc.r;
    drawnBuffer[y][x][1] = lc.g;
    drawnBuffer[y][x][2] = lc.b;
    mask[y][x] = true;
};

void WhiteBoard::SetBackData(struct color lc)
{
    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            if (!mask[y][x]) {
                SetFrameBufferPixel(x, y, lc);
            }
        }
    }
};

void WhiteBoard::SetFrameBuffer()
{
    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            SetFrameBufferPixel(x, y, backColor);
            mask[y][x] = false;
        }
    }
}

void WhiteBoard::ClearMaskData()
{
    memset(mask, 0, WINDOW_HEIGHT * WINDOW_WIDTH * sizeof(bool));
}

void WhiteBoard::Display()
{
    glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);
    // Copy the drawn pixels from the drawnBuffer to the frameBuffer
    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            if (mask[y][x]) {
                SetFrameBufferPixel(x, y, { drawnBuffer[y][x][0], drawnBuffer[y][x][1], drawnBuffer[y][x][2] });
            }
        }
    }
}


void WhiteBoard::DrawSquare(int xpos, int ypos, int xend, int yend, int size, struct color lc)
{

    int halfSize = size / 2;

    // Function to draw a filled rectangle (faster than per-pixel drawing)
    auto DrawRectangle = [&](int x, int y) {
        for (int row = x - halfSize; row <= x + halfSize; ++row) {
            for (int col = y - halfSize; col <= y + halfSize; ++col) {
                SetDrawnBufferPixel(col, row, lc);
            }
        }
    };

    if (xpos == xend && ypos == yend) {
        // If it's a single point, just draw the square
        DrawRectangle(xpos, ypos);
    }
    else {
        // Bresenham's Line Algorithm for connecting xpos, ypos to xend, yend
        int dx = std::abs(xend - xpos);
        int dy = std::abs(yend - ypos);
        int sx = (xpos < xend) ? 1 : -1;
        int sy = (ypos < yend) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            DrawRectangle(xpos, ypos); // Only draw once for each line pixel
            if (xpos == xend && ypos == yend) break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                xpos += sx;
            }
            if (e2 < dx) {
                err += dx;
                ypos += sy;
            }
        }
    }
}

void WhiteBoard::CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
    int framebufferX = WINDOW_HEIGHT - 1 - (int)ypos;  // Invert the new y-coordinate
    int framebufferY = (int)xpos;                     // Swap x and y

    int state = glfwGetMouseButton(lWindow, GLFW_MOUSE_BUTTON_LEFT);

    if (state == GLFW_PRESS) {
        if (!isMousePressed) {
            // Record the start position
            startX = framebufferX;
            startY = framebufferY;
            isMousePressed = true;
            std::cout << "Mouse pressed at: x  " << startX << ", y  " << startY << std::endl;
        }
        else if (!line) {
            // Draw as the mouse is pressed and dragged
            DrawSquare(framebufferX, framebufferY, framebufferX, framebufferY, brushSize, brushColor);
            std::cout << "Mouse dragging: x  " << framebufferX << ", y  " << framebufferY << std::endl;
        }
    }

    if (state == GLFW_RELEASE && isMousePressed) {
        // Record the end position
        endX = framebufferX;
        endY = framebufferY;
        isMousePressed = false;

        std::cout << "Mouse released at: x " << endX << ", y  " << endY << std::endl;

        if (line) {
            // Example for drawing a line between start and end points
            DrawSquare(startX, startY, endX, endY, brushSize, brushColor);
        }
    }
}


void WhiteBoard::MouseCallback(GLFWwindow* lWindow, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Mouse left button is pressed." << std::endl;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        SetFrameBuffer();
        ClearMaskData();  // Clear the mask data when resetting
        std::cout << "Mouse right button is pressed. Screen reset." << std::endl;
    }
}

void WhiteBoard::CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
    if (key == '+') {
        brushSize = CLAMP(brushSize * 2, 1, 128);
    }
    else if (key == '-') {
        brushSize = CLAMP(brushSize / 2, 1, 128);
    }
    else if (key >= '0' && key <= '7') {
        int colorIndex = key - '0';
        brushColor.r = (colorIndex & 0b100) ? 1.0f : 0.0f;
        brushColor.g = (colorIndex & 0b010) ? 1.0f : 0.0f;
        brushColor.b = (colorIndex & 0b001) ? 1.0f : 0.0f;
    }
    else if (key == 'l') {
        if (!isMousePressed) {
            line = (line ? false : true);
        }
    }
    std::cout << "Key " << (char)key << " is pressed." << std::endl;
}

// Static wrapper for CursorPositionCallback
void WhiteBoard::StaticCursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos) {
    WhiteBoard* whiteboard = static_cast<WhiteBoard*>(glfwGetWindowUserPointer(lWindow));
    if (whiteboard) {
        whiteboard->CursorPositionCallback(lWindow, xpos, ypos);
    }
}
// Static wrapper for MouseCallback
void WhiteBoard::StaticMouseCallback(GLFWwindow* lWindow, int button, int action, int mods) {
    WhiteBoard* whiteboard = static_cast<WhiteBoard*>(glfwGetWindowUserPointer(lWindow));
    if (whiteboard) {
        whiteboard->MouseCallback(lWindow, button, action, mods);
    }
}

// Static wrapper for CharacterCallback
void WhiteBoard::StaticCharacterCallback(GLFWwindow* lWindow, unsigned int key) {
    WhiteBoard* whiteboard = static_cast<WhiteBoard*>(glfwGetWindowUserPointer(lWindow));
    if (whiteboard) {
        whiteboard->CharacterCallback(lWindow, key);
    }
}
