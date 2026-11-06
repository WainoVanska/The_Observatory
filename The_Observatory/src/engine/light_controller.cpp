#include "light_controller.h"

LightController::LightController()
{
    position = glm::vec3(0.0f, 1.5f, 1.2f);
    speed = 0.75f; // movement speed in units per second
}

void LightController::update(GLFWwindow* window, float deltaTime)
{
    const float distance = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        position.z -= distance;
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        position.z += distance;
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        position.x -= distance;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        position.x += distance;
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        position.y += distance;
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        position.y -= distance;
    }
}

glm::vec3 LightController::getPosition() const
{
    return position;
}
