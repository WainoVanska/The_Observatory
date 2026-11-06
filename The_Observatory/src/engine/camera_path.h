#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <filesystem>
#include <vector>

// Where the camera is and where it looks. Yaw and pitch are in degrees, the same as in FreeCamera.
struct CameraPose
{
    glm::vec3 position;
    float yaw;
    float pitch;
};

struct CameraKeyframe
{
    CameraPose pose;
    float duration; // seconds it takes to travel to the next keyframe (unused on the last keyframe)
};

// A list of camera keyframes that the camera glides through along a smooth curve.
// Paths are stored as plain text files with one keyframe per line: x y z yaw pitch duration
class CameraPath
{
  public:
    static constexpr float DEFAULT_SEGMENT_DURATION = 2.0f;

    // Replaces the keyframes with the ones in the file. A missing file gives an empty path.
    bool load(const std::filesystem::path& filePath);
    bool save(const std::filesystem::path& filePath) const;

    void addKeyframe(const CameraPose& pose);
    void removeLastKeyframe();

    std::size_t getKeyframeCount() const;
    bool canPlay() const;

    // Total travel time from the first keyframe to the last, in seconds.
    float getDuration() const;

    // Returns the camera pose at the given time, where 0 is the first keyframe and getDuration() is the last.
    CameraPose evaluate(float time) const;

  private:
    // How long the camera takes to speed up at the start of the path and slow down at the end.
    static constexpr float EASE_DURATION = 0.75f;

    float applyEasing(float time) const;

    std::vector<CameraKeyframe> keyframes;
};
