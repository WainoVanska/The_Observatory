#pragma once

#include <glad/glad.h>
#include <string>

GLint getUniformLocation(GLuint program, const std::string& name);
void clearUniformLocationCache();

std::string readShaderFile(const std::string& filePath);

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

GLuint createShaderProgram(const std::string& vertexPath,
                           const std::string& geometryPath,
                           const std::string& fragmentPath);
