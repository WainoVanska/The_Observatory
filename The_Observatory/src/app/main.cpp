#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <system_error>

#include "../engine/camera_path.h"
#include "../engine/camera_path_player.h"
#include "../engine/free_camera.h"
#include "../engine/light_controller.h"
#include "../engine/window.h"
#include "../scenes/graphics_scene.h"
#include "../engine/frame_recorder.h"

namespace
{
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr const char* WINDOW_TITLE = "---";

// Limits the time step after a long stall (e.g. dragging the window) so nothing jumps across the scene.
constexpr float MAX_DELTA_TIME = 0.1f;

constexpr int SHOT_COUNT = 9;

const std::filesystem::path CAMERA_PATH_DIRECTORY = "camera_paths";
const std::filesystem::path RECORDING_DIRECTORY = "recordings";

std::filesystem::path getShotPathFile(int shot)
{
    return CAMERA_PATH_DIRECTORY / ("shot_" + std::to_string(shot) + ".txt");
}

std::filesystem::path getShotRecordingFile(int shot)
{
    return RECORDING_DIRECTORY / ("shot_" + std::to_string(shot) + ".mp4");
}

std::filesystem::path nextRecordingPath()
{
    std::error_code error;
    std::filesystem::create_directories(RECORDING_DIRECTORY, error);

    for (int number = 1;; ++number)
    {
        const std::filesystem::path path = RECORDING_DIRECTORY / ("observatory_" + std::to_string(number) + ".mp4");

        if (!std::filesystem::exists(path))
        {
            return path;
        }
    }
}

// The video encoder needs even dimensions, so odd framebuffer sizes are rounded down by one pixel.
glm::ivec2 getRecordingSize(GLFWwindow* window)
{
    int framebufferWidth;
    int framebufferHeight;

    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    return glm::ivec2(framebufferWidth - framebufferWidth % 2, framebufferHeight - framebufferHeight % 2);
}

bool startManualRecording(FrameRecorder& recorder, GLFWwindow* window)
{
    const glm::ivec2 recordingSize = getRecordingSize(window);
    const std::filesystem::path outputPath = nextRecordingPath();

    std::cout << "Recording resolution: " << recordingSize.x << "x" << recordingSize.y << '\n';

    if (!recorder.start(outputPath.string(), recordingSize.x, recordingSize.y, 60))
    {
        std::cerr << "Recorder failed to start.\n";
        return false;
    }

    std::cout << "Saving recording to: " << outputPath << '\n';
    return true;
}

// Reports a key only on the frame it goes down, so holding a key triggers its action once.
class KeyPress
{
  public:
    explicit KeyPress(int key) : key(key) {}

    bool wasJustPressed(GLFWwindow* window)
    {
        const bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
        const bool justPressed = isPressed && !wasPressed;

        wasPressed = isPressed;
        return justPressed;
    }

  private:
    int key;
    bool wasPressed = false;
};

// Shows the frame rate and the current status (selected shot, playback, recording) in the window title.
class WindowTitle
{
  public:
    WindowTitle() : previousTime(glfwGetTime()) {}

    void update(GLFWwindow* window, const std::string& status)
    {
        ++frameCount;

        const double currentTime = glfwGetTime();
        const double elapsedTime = currentTime - previousTime;
        bool titleChanged = status != previousStatus;

        if (elapsedTime >= 1.0)
        {
            framesPerSecond = static_cast<int>(frameCount / elapsedTime);
            frameCount = 0;
            previousTime = currentTime;
            titleChanged = true;
        }

        if (!titleChanged)
        {
            return;
        }

        const std::string title =
            std::string(WINDOW_TITLE) + " | FPS: " + std::to_string(framesPerSecond) + " | " + status;

        glfwSetWindowTitle(window, title.c_str());
        previousStatus = status;
    }

  private:
    double previousTime;
    int frameCount = 0;
    int framesPerSecond = 0;
    std::string previousStatus;
};

std::string createStatusText(int shot,
                             const CameraPath& shotPath,
                             const CameraPathPlayer& pathPlayer,
                             const FrameRecorder& recorder)
{
    std::ostringstream status;

    status << "Shot " << shot << " | ";

    if (pathPlayer.isPlaying())
    {
        status << std::fixed << std::setprecision(1) << "Playing " << pathPlayer.getPlaybackTime() << " / "
               << pathPlayer.getTotalDuration() << " s";
    }
    else
    {
        status << shotPath.getKeyframeCount() << " keyframes";
    }

    if (recorder.isRecording())
    {
        status << " | REC";
    }

    return status.str();
}
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
    WindowTitle windowTitle;

    FrameRecorder recorder;
    CameraPathPlayer pathPlayer(recorder);

    // The shot files are the source of truth: they are re-read before every change and playback,
    // so a path can be edited in a text editor while the application is running.
    int currentShot = 1;
    CameraPath shotPath;
    shotPath.load(getShotPathFile(currentShot));

    KeyPress recordKey(GLFW_KEY_R);
    KeyPress previousShotKey(GLFW_KEY_LEFT_BRACKET);
    KeyPress nextShotKey(GLFW_KEY_RIGHT_BRACKET);
    KeyPress addKeyframeKey(GLFW_KEY_C);
    KeyPress removeKeyframeKey(GLFW_KEY_BACKSPACE);
    KeyPress playShotKey(GLFW_KEY_T);

    constexpr double RECORDING_INTERVAL = 1.0 / 60.0;
    double nextRecordingFrame = 0.0;

    double previousFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        const double frameTime = glfwGetTime();
        const float realDeltaTime = glm::min(static_cast<float>(frameTime - previousFrameTime), MAX_DELTA_TIME);
        previousFrameTime = frameTime;

        // Every key is checked on every frame, so a key held down during playback doesn't trigger afterwards.
        const bool recordPressed = recordKey.wasJustPressed(window);
        const bool previousShotPressed = previousShotKey.wasJustPressed(window);
        const bool nextShotPressed = nextShotKey.wasJustPressed(window);
        const bool addKeyframePressed = addKeyframeKey.wasJustPressed(window);
        const bool removeKeyframePressed = removeKeyframeKey.wasJustPressed(window);
        const bool playShotPressed = playShotKey.wasJustPressed(window);

        if (playShotPressed && pathPlayer.isPlaying())
        {
            pathPlayer.cancel();
        }
        else if (playShotPressed)
        {
            shotPath.load(getShotPathFile(currentShot));

            const glm::ivec2 recordingSize = getRecordingSize(window);
            const std::filesystem::path outputPath = getShotRecordingFile(currentShot);

            if (!shotPath.canPlay())
            {
                std::cout << "Shot " << currentShot << " needs at least 2 keyframes to play.\n";
            }
            else if (recorder.isRecording())
            {
                std::cout << "Stop the manual recording (R) before playing a shot.\n";
            }
            else if (pathPlayer.start(shotPath, outputPath, recordingSize.x, recordingSize.y))
            {
                std::cout << "Playing shot " << currentShot << ", saving to " << outputPath << '\n';
            }
            else
            {
                std::cerr << "Could not start recording shot " << currentShot << ".\n";
            }
        }

        if (!pathPlayer.isPlaying())
        {
            if (previousShotPressed || nextShotPressed)
            {
                currentShot = previousShotPressed ? currentShot - 1 : currentShot + 1;
                currentShot = (currentShot + SHOT_COUNT - 1) % SHOT_COUNT + 1; // wraps around to stay within 1-9

                shotPath.load(getShotPathFile(currentShot));
                std::cout << "Selected shot " << currentShot << " (" << shotPath.getKeyframeCount() << " keyframes)\n";
            }

            if (addKeyframePressed || removeKeyframePressed)
            {
                shotPath.load(getShotPathFile(currentShot));

                if (addKeyframePressed)
                {
                    shotPath.addKeyframe(CameraPose{camera.getPosition(), camera.yaw, camera.pitch});
                }
                else
                {
                    shotPath.removeLastKeyframe();
                }

                shotPath.save(getShotPathFile(currentShot));
                std::cout << "Shot " << currentShot << " now has " << shotPath.getKeyframeCount() << " keyframes\n";
            }

            if (recordPressed && recorder.isRecording())
            {
                recorder.stop();
            }
            else if (recordPressed && startManualRecording(recorder, window))
            {
                nextRecordingFrame = glfwGetTime();
            }
        }

        // During playback every frame moves the scene forward by exactly one video frame,
        // no matter how long the frame took to render.
        const float deltaTime = pathPlayer.isPlaying() ? CameraPathPlayer::FRAME_DURATION : realDeltaTime;

        if (pathPlayer.isPlaying())
        {
            pathPlayer.applyCameraPose(camera);
        }
        else
        {
            camera.update(window, deltaTime);
        }

        lightController.update(window, deltaTime);
        scene.handleInput(window, deltaTime);
        scene.advanceTime(deltaTime);

        scene.draw(camera.getViewMatrix(),
                   projection,
                   camera.getPosition(),
                   lightController.getPosition(),
                   camera.getFront());

        if (pathPlayer.isPlaying())
        {
            pathPlayer.finishFrame();
        }
        else if (recorder.isRecording())
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

        windowTitle.update(window, createStatusText(currentShot, shotPath, pathPlayer, recorder));
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    // Closing the window mid-playback leaves an unfinished shot, which is removed like a cancelled one.
    pathPlayer.cancel();
    recorder.stop();
    scene.cleanup();
    glfwTerminate();
    return 0;
}
