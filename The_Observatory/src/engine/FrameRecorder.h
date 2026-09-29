#pragma once

#include <cstdio>
#include <string>
#include <vector>

class FrameRecorder
{
  public:
    FrameRecorder() = default;
    ~FrameRecorder();

    bool start(const std::string& outputPath, int width, int height, int frameRate = 60);

    void captureFrame();
    void stop();

    bool isRecording() const;

  private:
    FILE* ffmpegPipe = nullptr;

    int width = 0;
    int height = 0;

    std::vector<unsigned char> pixelBuffer;
};
