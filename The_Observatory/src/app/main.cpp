#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>
#include <filesystem>

#include "../engine/free_camera.h"
#include "../engine/light_controller.h"
#include "../engine/window.h"
#include "../scenes/graphics_scene.h"
#include "../engine/FrameRecorder.h"

namespace
{
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr const char* WINDOW_TITLE = "---";

// Limits the time step after a long stall (e.g. dragging the window) so nothing jumps across the scene.
constexpr float MAX_DELTA_TIME = 0.1f;

std::filesystem::path nextRecordingPath()
{
    for (int number = 1;; ++number)
    {
        const std::filesystem::path path =
            std::filesystem::current_path() / ("observatory_" + std::to_string(number) + ".mp4");

        if (!std::filesystem::exists(path))
        {
            return path;
        }
    }
}

class FrameRateTitle
{
  public:
    FrameRateTitle() : previousTime(glfwGetTime()) {}

    void update(GLFWwindow* window, bool isRecording)
    {
        ++frameCount;

        const double currentTime = glfwGetTime();
        const double elapsedTime = currentTime - previousTime;

        if (elapsedTime < 1.0)
        {
            return;
        }

        const double framesPerSecond = frameCount / elapsedTime;

        std::string title = std::string(WINDOW_TITLE) + " | FPS: " + std::to_string(static_cast<int>(framesPerSecond));

        if (isRecording)
        {
            title += " | REC";
        }

        glfwSetWindowTitle(window, title.c_str());

        frameCount = 0;
        previousTime = currentTime;
    }

  private:
    double previousTime;
    int frameCount = 0;
};
} // namespace

int main()
{
    GLFWwindow* window = createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    if (!window)
    {
        return -1;
    }

    const glm::mat4 projection = glm::perspective(glm::radians(70.0f),
                                                  static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
                                                  0.1f,
                                                  250.0f);

    FreeCamera camera;
    LightController lightController;

    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();

    GraphicsScene scene;
    scene.init(WINDOW_WIDTH, WINDOW_HEIGHT, &camera);
    FrameRateTitle frameRateTitle;

    FrameRecorder recorder;
    bool recordKeyWasPressed = false;

    constexpr double RECORDING_INTERVAL = 1.0 / 60.0;
    double nextRecordingFrame = 0.0;

    double previousFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        const double frameTime = glfwGetTime();
        const float deltaTime = glm::min(static_cast<float>(frameTime - previousFrameTime), MAX_DELTA_TIME);
        previousFrameTime = frameTime;

        camera.update(window, deltaTime);
        lightController.update(window, deltaTime);
        scene.handleInput(window, deltaTime);

        const bool recordKeyPressed = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

        if (recordKeyPressed && !recordKeyWasPressed)
        {
            if (recorder.isRecording())
            {
                recorder.stop();
            }
            else
            {
                int framebufferWidth;
                int framebufferHeight;

                glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

                std::cout << "Original framebuffer: " << framebufferWidth << "x" << framebufferHeight << '\n';

                // Round odd dimensions by one pixel
                framebufferWidth -= framebufferWidth % 2;
                framebufferHeight -= framebufferHeight % 2;

                std::cout << "Recording resolution: " << framebufferWidth << "x" << framebufferHeight << '\n';

                const std::filesystem::path outputPath = nextRecordingPath();

                const bool started = recorder.start(outputPath.string(), framebufferWidth, framebufferHeight, 60);

                if (started)
                {
                    nextRecordingFrame = glfwGetTime();

                    std::cout << "Saving recording to: " << outputPath << '\n';
                }
                else
                {
                    std::cerr << "Recorder failed to start.\n";
                }
            }
        }

        recordKeyWasPressed = recordKeyPressed;

        scene.draw(camera.getViewMatrix(),
                   projection,
                   camera.getPosition(),
                   lightController.getPosition(),
                   camera.getFront());

        if (recorder.isRecording())
        {
            const double currentTime = glfwGetTime();

            if (currentTime >= nextRecordingFrame)
            {
                recorder.captureFrame();
                nextRecordingFrame += RECORDING_INTERVAL;

                // Prevent trying to catch up after a large delay.
                if (nextRecordingFrame < currentTime)
                {
                    nextRecordingFrame = currentTime + RECORDING_INTERVAL;
                }
            }
        }

        frameRateTitle.update(window, recorder.isRecording());
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    recorder.stop();
    scene.cleanup();
    glfwTerminate();
    return 0;
}
