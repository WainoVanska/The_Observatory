#include "light_controller.h"

LightController::LightController()
{
    position = glm::vec3(0.0f, 1.5f, 1.2f);
    speed = 0.02f;
}

void LightController::update(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        position.z -= speed;
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        position.z += speed;
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        position.x -= speed;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        position.x += speed;
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        position.y += speed;
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        position.y -= speed;
    }
}

glm::vec3 LightController::getPosition() const
{
    return position;
}