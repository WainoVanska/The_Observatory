#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class FreeCamera
{
  public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraRight;
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    bool firstMouse;

    float yaw;
    float pitch;
    float cameraSpeed;
    float sensitivity;

    double lastMousePosX;
    double lastMousePosY;

    FreeCamera();

    void update(GLFWwindow* window);
    glm::mat4 getViewMatrix();
};
