#include "camera_path_player.h"

#include <cmath>
#include <iostream>
#include <system_error>

CameraPathPlayer::CameraPathPlayer(FrameRecorder& recorder) : recorder(recorder) {}

bool CameraPathPlayer::start(const CameraPath& newPath,
                             const std::filesystem::path& newOutputPath,
                             int width,
                             int height)
{
    if (playing || recorder.isRecording() || !newPath.canPlay())
    {
        return false;
    }

    std::error_code error;
    std::filesystem::create_directories(newOutputPath.parent_path(), error);

    if (!recorder.start(newOutputPath.string(), width, height, FRAME_RATE))
    {
        return false;
    }

    path = newPath;
    outputPath = newOutputPath;
    playing = true;
    frameIndex = 0;

    const float totalDuration = HOLD_DURATION + path.getDuration() + HOLD_DURATION;

    // One extra frame so the very last frame lands exactly on the end of the path.
    frameCount = static_cast<int>(std::round(totalDuration * FRAME_RATE)) + 1;

    return true;
}

void CameraPathPlayer::cancel()
{
    if (!playing)
    {
        return;
    }

    stop();

    std::error_code error;
    std::filesystem::remove(outputPath, error);

    std::cout << "Playback cancelled, removed " << outputPath << '\n';
}

bool CameraPathPlayer::isPlaying() const
{
    return playing;
}

float CameraPathPlayer::getPlaybackTime() const
{
    return frameIndex * FRAME_DURATION;
}

float CameraPathPlayer::getTotalDuration() const
{
    return (frameCount - 1) * FRAME_DURATION;
}

void CameraPathPlayer::applyCameraPose(FreeCamera& camera) const
{
    if (!playing)
    {
        return;
    }

    const CameraPose pose = path.evaluate(getPlaybackTime() - HOLD_DURATION);

    camera.setPose(pose.position, pose.yaw, pose.pitch);
}

void CameraPathPlayer::finishFrame()
{
    if (!playing)
    {
        return;
    }

    recorder.captureFrame();
    ++frameIndex;

    if (frameIndex >= frameCount)
    {
        stop();

        std::cout << "Saved shot to " << outputPath << '\n';
    }
}

void CameraPathPlayer::stop()
{
    recorder.stop();
    playing = false;
}
