#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

#include "../common/free_camera.h"
#include "../common/light_controller.h"
#include "../common/window.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "week09_scene.h"

int main()
{
    const int windowWidth = 800;
    const int windowHeight = 600;

    GLFWwindow* window = createWindow(windowWidth, windowHeight, "W9-W11");

    if (!window) {
        return -1;
    }

    glm::mat4 projection = glm::perspective(
        glm::radians(70.0f),
        (float)windowWidth / (float)windowHeight,
        0.1f,
        100.0f
    );

    FreeCamera camera;
    LightController lightController;

    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();

    Week09Scene scene;
    scene.init(windowWidth, windowHeight, &camera);

    double previousTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        camera.update(window);
        lightController.update(window);
        scene.handleInput(window);

        glm::mat4 view = camera.getViewMatrix();
        glm::vec3 cameraPos = camera.getPosition();
        glm::vec3 movableLightPos = lightController.getPosition();
        glm::vec3 cameraFront = camera.getFront();

        scene.draw(view, projection, cameraPos, movableLightPos, cameraFront);

        // fps tracker

        frameCount++;

        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - previousTime;

        if (elapsedTime >= 1.0)
        {
            double fps = frameCount / elapsedTime;

            std::string title = "W9-W11 | FPS: " + std::to_string((int)fps);
            glfwSetWindowTitle(window, title.c_str());

            frameCount = 0;
            previousTime = currentTime;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    scene.cleanup();

    glfwTerminate();
    return 0;
}