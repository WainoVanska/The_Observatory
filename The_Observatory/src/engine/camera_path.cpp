#include "camera_path.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

namespace
{
// Distance between two knots of a centripetal Catmull-Rom curve. The minimum keeps the math stable
// when two keyframes share the same value (e.g. the camera only turns, but doesn't move).
template <typename Vector> float knotInterval(const Vector& from, const Vector& to)
{
    return glm::max(std::sqrt(glm::length(to - from)), 0.0001f);
}

// Centripetal Catmull-Rom curve between p1 and p2, where u goes from 0 (at p1) to 1 (at p2).
// p0 and p3 are the neighbouring keyframes and only shape the curve. Unlike the uniform version,
// the centripetal curve doesn't overshoot or form loops between keyframes.
template <typename Vector>
Vector catmullRom(const Vector& p0, const Vector& p1, const Vector& p2, const Vector& p3, float u)
{
    const float t0 = 0.0f;
    const float t1 = t0 + knotInterval(p0, p1);
    const float t2 = t1 + knotInterval(p1, p2);
    const float t3 = t2 + knotInterval(p2, p3);
    const float t = glm::mix(t1, t2, u);

    const Vector a1 = ((t1 - t) * p0 + (t - t0) * p1) / (t1 - t0);
    const Vector a2 = ((t2 - t) * p1 + (t - t1) * p2) / (t2 - t1);
    const Vector a3 = ((t3 - t) * p2 + (t - t2) * p3) / (t3 - t2);

    const Vector b1 = ((t2 - t) * a1 + (t - t0) * a2) / (t2 - t0);
    const Vector b2 = ((t3 - t) * a2 + (t - t1) * a3) / (t3 - t1);

    return ((t2 - t) * b1 + (t - t1) * b2) / (t2 - t1);
}

glm::vec2 getAngles(const CameraPose& pose)
{
    return glm::vec2(pose.yaw, pose.pitch);
}

// The first and last keyframes have a neighbour on one side only.
// The missing neighbour is replaced with the other neighbour mirrored around the keyframe.
CameraPose mirrorPose(const CameraPose& center, const CameraPose& other)
{
    return CameraPose{2.0f * center.position - other.position,
                      2.0f * center.yaw - other.yaw,
                      2.0f * center.pitch - other.pitch};
}

// Wraps an angle in degrees to the range [-180, 180].
float wrapDegrees(float angle)
{
    return angle - 360.0f * std::round(angle / 360.0f);
}

bool isCommentOrEmpty(const std::string& line)
{
    const std::size_t firstCharacter = line.find_first_not_of(" \t\r");

    return firstCharacter == std::string::npos || line[firstCharacter] == '#';
}
} // namespace

bool CameraPath::load(const std::filesystem::path& filePath)
{
    keyframes.clear();

    std::ifstream file(filePath);

    // A shot that hasn't been created yet is simply an empty path.
    if (!file)
    {
        return true;
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line))
    {
        ++lineNumber;

        if (isCommentOrEmpty(line))
        {
            continue;
        }

        std::istringstream stream(line);
        CameraKeyframe keyframe{};

        stream >> keyframe.pose.position.x >> keyframe.pose.position.y >> keyframe.pose.position.z >>
            keyframe.pose.yaw >> keyframe.pose.pitch >> keyframe.duration;

        if (!stream)
        {
            std::cerr << "Invalid keyframe on line " << lineNumber << " of " << filePath << ".\n";
            keyframes.clear();
            return false;
        }

        keyframe.duration = glm::max(keyframe.duration, 0.0f);
        keyframes.push_back(keyframe);
    }

    return true;
}

bool CameraPath::save(const std::filesystem::path& filePath) const
{
    std::error_code error;
    std::filesystem::create_directories(filePath.parent_path(), error);

    std::ofstream file(filePath);

    if (!file)
    {
        std::cerr << "Could not save the camera path to " << filePath << ".\n";
        return false;
    }

    file << "# Camera path for The Observatory\n"
         << "# One keyframe per line: x y z yaw pitch duration\n"
         << "# duration = seconds to travel to the next keyframe (unused on the last keyframe)\n";

    file << std::fixed << std::setprecision(3);

    for (const CameraKeyframe& keyframe : keyframes)
    {
        const CameraPose& pose = keyframe.pose;

        file << pose.position.x << ' ' << pose.position.y << ' ' << pose.position.z << ' ' << pose.yaw << ' '
             << pose.pitch << ' ' << keyframe.duration << '\n';
    }

    return true;
}

void CameraPath::addKeyframe(const CameraPose& pose)
{
    CameraKeyframe keyframe{pose, DEFAULT_SEGMENT_DURATION};

    // Mouse look lets the yaw grow past 360 degrees. Keeping the new yaw within half a turn of the
    // previous keyframe makes the camera turn the short way instead of spinning around.
    if (!keyframes.empty())
    {
        const float previousYaw = keyframes.back().pose.yaw;

        keyframe.pose.yaw = previousYaw + wrapDegrees(pose.yaw - previousYaw);
    }

    keyframes.push_back(keyframe);
}

void CameraPath::removeLastKeyframe()
{
    if (!keyframes.empty())
    {
        keyframes.pop_back();
    }
}

std::size_t CameraPath::getKeyframeCount() const
{
    return keyframes.size();
}

bool CameraPath::canPlay() const
{
    return keyframes.size() >= 2;
}

float CameraPath::getDuration() const
{
    float duration = 0.0f;

    // The last keyframe has nowhere to travel, so its duration isn't counted.
    for (std::size_t index = 0; index + 1 < keyframes.size(); ++index)
    {
        duration += keyframes[index].duration;
    }

    return duration;
}

CameraPose CameraPath::evaluate(float time) const
{
    if (keyframes.empty())
    {
        return CameraPose{glm::vec3(0.0f), -90.0f, 0.0f};
    }

    if (keyframes.size() == 1)
    {
        return keyframes.front().pose;
    }

    float segmentTime = applyEasing(glm::clamp(time, 0.0f, getDuration()));

    // Find the segment (the part of the path between two keyframes) that the time falls into.
    std::size_t segment = 0;

    while (segment + 2 < keyframes.size() && segmentTime > keyframes[segment].duration)
    {
        segmentTime -= keyframes[segment].duration;
        ++segment;
    }

    const float segmentDuration = keyframes[segment].duration;
    const float progress = segmentDuration > 0.0f ? glm::clamp(segmentTime / segmentDuration, 0.0f, 1.0f) : 1.0f;

    const CameraPose& current = keyframes[segment].pose;
    const CameraPose& next = keyframes[segment + 1].pose;
    const CameraPose previous = segment > 0 ? keyframes[segment - 1].pose : mirrorPose(current, next);
    const CameraPose afterNext =
        segment + 2 < keyframes.size() ? keyframes[segment + 2].pose : mirrorPose(next, current);

    const glm::vec3 position =
        catmullRom(previous.position, current.position, next.position, afterNext.position, progress);
    const glm::vec2 angles =
        catmullRom(getAngles(previous), getAngles(current), getAngles(next), getAngles(afterNext), progress);

    return CameraPose{position, angles.x, glm::clamp(angles.y, -89.0f, 89.0f)};
}

// Remaps the playback time so the camera speeds up gently at the start of the path and slows down
// at the end, while moving at a steady pace in between. The total duration stays the same.
float CameraPath::applyEasing(float time) const
{
    const float duration = getDuration();
    const float easeDuration = glm::min(EASE_DURATION, duration * 0.5f);

    if (easeDuration <= 0.0f)
    {
        return time;
    }

    // The middle part has to move a little faster to make up for the slower start and end.
    const float peakSpeed = duration / (duration - easeDuration);

    if (time < easeDuration)
    {
        return peakSpeed * time * time / (2.0f * easeDuration);
    }

    if (time > duration - easeDuration)
    {
        const float remainingTime = duration - time;

        return duration - peakSpeed * remainingTime * remainingTime / (2.0f * easeDuration);
    }

    return peakSpeed * (time - easeDuration * 0.5f);
}
