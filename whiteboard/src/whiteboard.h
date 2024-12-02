#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>



#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

class WhiteBoard {

	struct color { float r, g, b; };

	public:
		WhiteBoard(float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], float drawnBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3], bool mask[WINDOW_HEIGHT][WINDOW_WIDTH], GLFWwindow* window);
		void SetFrameBufferPixel(int x, int y, struct color lc);
		void SetDrawnBufferPixel(int x, int y, struct color lc);
		void SetBackData(struct color lc);
		void SetFrameBuffer();
		void ClearMaskData();
		void Display();
		void DrawSquare(int xpos, int ypos, int xend, int yend, int size, struct color lc);
		void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos);
		void MouseCallback(GLFWwindow* lWindow, int button, int action, int mods);
		void CharacterCallback(GLFWwindow* lWindow, unsigned int key);

		static void StaticCursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos);
		static void StaticMouseCallback(GLFWwindow* lWindow, int button, int action, int mods);
		static void StaticCharacterCallback(GLFWwindow* lWindow, unsigned int key);

	private:
		bool line;
		bool isMousePressed;
		int startX, startY;
		int endX, endY;
		int brushSize;
		float(*frameBuffer)[WINDOW_WIDTH][3];
		float(*drawnBuffer)[WINDOW_WIDTH][3];
		bool(*mask)[WINDOW_WIDTH];
		struct color brushColor;
		struct color backColor;
		GLFWwindow* window;
};


