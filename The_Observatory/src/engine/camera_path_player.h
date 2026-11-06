#pragma once

#include <filesystem>

#include "camera_path.h"
#include "free_camera.h"
#include "frame_recorder.h"

// Plays a camera path and records it to a video file.
// Playback advances by exactly one video frame per rendered frame, so the video stays smooth
// even when the scene renders slower than the video frame rate.
class CameraPathPlayer
{
  public:
    static constexpr int FRAME_RATE = 60;
    static constexpr float FRAME_DURATION = 1.0f / FRAME_RATE;

    explicit CameraPathPlayer(FrameRecorder& recorder);

    bool start(const CameraPath& path, const std::filesystem::path& outputPath, int width, int height);

    // Stops the playback and deletes the unfinished video.
    void cancel();

    bool isPlaying() const;
    float getPlaybackTime() const;
    float getTotalDuration() const;

    // Moves the camera to where it should be on the current frame. Call before drawing the scene.
    void applyCameraPose(FreeCamera& camera) const;

    // Records the drawn frame and moves on to the next one. Call after drawing the scene.
    void finishFrame();

  private:
    // The camera holds still for a moment at the start and end, which gives clean cuts when editing.
    static constexpr float HOLD_DURATION = 0.5f;

    void stop();

    FrameRecorder& recorder;
    CameraPath path;
    std::filesystem::path outputPath;

    bool playing = false;
    int frameIndex = 0;
    int frameCount = 0;
};
