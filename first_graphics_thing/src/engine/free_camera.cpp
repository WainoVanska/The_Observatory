#include "free_camera.h"

FreeCamera::FreeCamera()
{
    cameraPos = glm::vec3(0.0f, 1.5f, 3.0f);         // the place in 3D space where the camera is located 
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

    yaw = -90.0f;               // turning left / right ("FPS POV" around the Y-axis)
    pitch = 0.0f;               // looking up / down ( around the X-axis)
    cameraSpeed = 0.01f;        // sensitivity of the mouse
    sensitivity = 0.05f;

    bool firstMouse;
    lastMousePosX = 0.0f;
    lastMousePosY = 0.0f;
}

void FreeCamera::update(GLFWwindow* window)
{
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));       // gives perpendicular vector to the camera (vec to the right)

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraRight;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraRight;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos.y += cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        cameraPos.y -= cameraSpeed;
    }

    double mousePosX;
    double mousePosY;

    glfwGetCursorPos(window, &mousePosX, &mousePosY);

    if (firstMouse)
    {
        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;
        firstMouse = false;
    }

    float mouseOffsetX = mousePosX - lastMousePosX;
    float mouseOffsetY = mousePosY - lastMousePosY;

    lastMousePosX = mousePosX;
    lastMousePosY = mousePosY;

    yaw += mouseOffsetX * sensitivity;
    pitch -= mouseOffsetY * sensitivity;

    if (pitch > 89.0f)
        pitch = 89.0f;

    if (pitch < -89.0f)
        pitch = -89.0f;

    //converts the mouse's x and y values to angles --> then make them direction vectors
    cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront.y = sin(glm::radians(pitch));
    cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(cameraFront);          // normalize back to [0,1] range
}

glm::mat4 FreeCamera::getViewMatrix()
{
    return glm::lookAt(
        cameraPos,                                  // camera position, where it's placed
        cameraPos + cameraFront,                    // look at character, what direction the camera is in
        glm::vec3(0.0f, 1.0f, 0.0f)                 // orientation, basically rotation of the camera itself
    );
}       

glm::vec3 FreeCamera::getPosition() const
{
    return cameraPos;
}

glm::vec3 FreeCamera::getFront() const
{
    return cameraFront;
}