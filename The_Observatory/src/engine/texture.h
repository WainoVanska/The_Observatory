#pragma once

#include <glad/glad.h>
#include <string>

GLuint loadTexture(const std::string& path, bool useSRGB = false);

GLuint loadTransparentTexture(const std::string& path, bool useSRGB = false);
