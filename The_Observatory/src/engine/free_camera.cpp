#include "free_camera.h"

FreeCamera::FreeCamera()
{
    cameraPos = glm::vec3(0.0f, 1.5f, 2.7f); // the place in 3D space where the camera is located
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

    yaw = -90.0f;        // turning left / right ("FPS POV" around the Y-axis)
    pitch = 0.0f;        // looking up / down ( around the X-axis)
    cameraSpeed = 1.5f;  // movement speed in units per second
    sensitivity = 0.05f; // sensitivity of the mouse

    firstMouse = true;
    lastMousePosX = 0.0f;
    lastMousePosY = 0.0f;
}

void FreeCamera::update(GLFWwindow* window, float deltaTime)
{
    glm::vec3 cameraRight = glm::normalize(
        glm::cross(cameraFront,
                   glm::vec3(0.0f, 1.0f, 0.0f))); // gives perpendicular vector to the camera (vec to the right)

    // Holding left shift slows the camera down for precise positioning
    float speedMultiplier = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        speedMultiplier = SLOW_SPEED_MULTIPLIER;
    }

    const float distance = cameraSpeed * speedMultiplier * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += distance * cameraFront;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= distance * cameraFront;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += distance * cameraRight;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= distance * cameraRight;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cameraPos.y += distance;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        cameraPos.y -= distance;
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

    const float mouseOffsetX = static_cast<float>(mousePosX - lastMousePosX);
    const float mouseOffsetY = static_cast<float>(mousePosY - lastMousePosY);

    lastMousePosX = mousePosX;
    lastMousePosY = mousePosY;

    yaw += mouseOffsetX * sensitivity;
    pitch -= mouseOffsetY * sensitivity;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }

    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    updateFront();
}

void FreeCamera::setPose(const glm::vec3& position, float newYaw, float newPitch)
{
    cameraPos = position;
    yaw = newYaw;
    pitch = newPitch;

    updateFront();

    // The mouse may have moved while the camera was placed from code.
    // Starting fresh avoids a sudden jump when mouse look takes over again.
    firstMouse = true;
}

void FreeCamera::updateFront()
{
    // converts the yaw and pitch angles into a direction vector
    cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront.y = sin(glm::radians(pitch));
    cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(cameraFront); // normalize back to [0,1] range
}

glm::mat4 FreeCamera::getViewMatrix()
{
    return glm::lookAt(cameraPos,                  // camera position, where it's placed
                       cameraPos + cameraFront,    // look at character, what direction the camera is in
                       glm::vec3(0.0f, 1.0f, 0.0f) // orientation, basically rotation of the camera itself
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
