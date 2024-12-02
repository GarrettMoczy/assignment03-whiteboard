# P2P Whiteboard Application

An OpenGL-based whiteboard application that supports real-time drawing with various features:
- Continuous line drawing
- Multiple colors
- Adjustable brush sizes
- Background color customization
- Console output for drawing data

## Prerequisites
- OpenGL
- GLEW
- GLFW3
- CMake (3.10 or higher)
- C++ compiler with C++17 support

## Building
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Controls
- Left Mouse Button: Draw
- Right Mouse Button: Clear canvas
- '+' : Increase brush size
- '-' : Decrease brush size
- '0-7': Change brush colors
- Shift + (!@#$%^&*() : Change background colors