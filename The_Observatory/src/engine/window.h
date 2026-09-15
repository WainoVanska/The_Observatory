#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* createWindow(
    int width,
    int height,
    const char* title
);