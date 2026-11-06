#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class LightController
{
  public:
    LightController();

    void update(GLFWwindow* window, float deltaTime);
    glm::vec3 getPosition() const;

  private:
    glm::vec3 position;
    float speed;
};
