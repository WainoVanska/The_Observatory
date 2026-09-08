#include "FrameRecorder.h"

#include <glad/glad.h>

#include <iostream>
#include <sstream>

#ifdef _WIN32
#define OPEN_PIPE _popen
#define CLOSE_PIPE _pclose
#else
#define OPEN_PIPE popen
#define CLOSE_PIPE pclose
#endif

FrameRecorder::~FrameRecorder()
{
    stop();
}

bool FrameRecorder::start(
    const std::string& outputPath,
    int recordingWidth,
    int recordingHeight,
    int frameRate
)
{
    if (isRecording())
    {
        std::cerr << "FrameRecorder is already recording.\n";
        return false;
    }

    if (recordingWidth <= 0 || recordingHeight <= 0 || frameRate <= 0)
    {
        std::cerr << "Invalid recording dimensions or frame rate.\n";
        return false;
    }

    if (recordingWidth % 2 != 0 || recordingHeight % 2 != 0)
    {
        std::cerr << "Recording width and height must be even numbers.\n";
        return false;
    }

    width = recordingWidth;
    height = recordingHeight;

    pixelBuffer.resize(
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height) *
        3
    );

    std::ostringstream command;

    const std::string ffmpegPath =
        R"(C:\Users\xcove\AppData\Local\Microsoft\WinGet\Packages\Gyan.FFmpeg_Microsoft.Winget.Source_8wekyb3d8bbwe\ffmpeg-8.1.2-full_build\bin\ffmpeg.exe)";

    command
        << ffmpegPath << " -y "
        << "-f rawvideo "
        << "-pixel_format rgb24 "
        << "-video_size " << width << "x" << height << " "
        << "-framerate " << frameRate << " "
        << "-i - "
        << "-vf vflip "
        << "-an "
        << "-c:v libx264 "
        << "-preset veryfast "
        << "-crf 18 "
        << "-pix_fmt yuv420p "
        << "\"" << outputPath << "\"";

    ffmpegPipe = OPEN_PIPE(command.str().c_str(), "wb");

    if (ffmpegPipe == nullptr)
    {
        std::cerr << "Could not start FFmpeg.\n";
        pixelBuffer.clear();
        return false;
    }

    std::cout << "Recording started: " << outputPath << '\n';
    return true;
}

void FrameRecorder::captureFrame()
{
    if (!isRecording())
    {
        return;
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);

    glReadPixels(
        0,
        0,
        width,
        height,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        pixelBuffer.data()
    );

    const std::size_t writtenBytes = std::fwrite(
        pixelBuffer.data(),
        1,
        pixelBuffer.size(),
        ffmpegPipe
    );

    if (writtenBytes != pixelBuffer.size())
    {
        std::cerr << "Failed to send a frame to FFmpeg.\n";
        stop();
    }
}

void FrameRecorder::stop()
{
    if (!isRecording())
    {
        return;
    }

    CLOSE_PIPE(ffmpegPipe);
    ffmpegPipe = nullptr;

    pixelBuffer.clear();
    width = 0;
    height = 0;

    std::cout << "Recording stopped.\n";
}

bool FrameRecorder::isRecording() const
{
    return ffmpegPipe != nullptr;
}