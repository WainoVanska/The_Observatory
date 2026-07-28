#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

#include "../engine/free_camera.h"
#include "../engine/light_controller.h"
#include "../engine/window.h"
#include "../scenes/graphics_scene.h"

namespace
{
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    constexpr const char* WINDOW_TITLE = "W9-W11";

    class FrameRateTitle
    {
    public:
        FrameRateTitle()
            : previousTime(glfwGetTime())
        {
        }

        void update(GLFWwindow* window)
        {
            ++frameCount;

            const double currentTime = glfwGetTime();
            const double elapsedTime = currentTime - previousTime;
            if (elapsedTime < 1.0)
            {
                return;
            }

            const double framesPerSecond = frameCount / elapsedTime;
            const std::string title =
                std::string(WINDOW_TITLE) + " | FPS: " +
                std::to_string(static_cast<int>(framesPerSecond));
            glfwSetWindowTitle(window, title.c_str());

            frameCount = 0;
            previousTime = currentTime;
        }

    private:
        double previousTime;
        int frameCount = 0;
    };
}

int main()
{
    GLFWwindow* window = createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    if (!window)
    {
        return -1;
    }

    const glm::mat4 projection = glm::perspective(
        glm::radians(70.0f),
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
        0.1f,
        100.0f);

    FreeCamera camera;
    LightController lightController;

    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();

    GraphicsScene scene;
    scene.init(WINDOW_WIDTH, WINDOW_HEIGHT, &camera);
    FrameRateTitle frameRateTitle;

    while (!glfwWindowShouldClose(window))
    {
        camera.update(window);
        lightController.update(window);
        scene.handleInput(window);

        scene.draw(
            camera.getViewMatrix(),
            projection,
            camera.getPosition(),
            lightController.getPosition(),
            camera.getFront());

        frameRateTitle.update(window);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    scene.cleanup();
    glfwTerminate();
    return 0;
}
