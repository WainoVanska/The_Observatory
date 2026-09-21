#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    GLuint compileShader(GLenum type, const std::string& source)
    {
        const GLuint shader = glCreateShader(type);
        const char* sourcePointer = source.c_str();
        glShaderSource(shader, 1, &sourcePointer, nullptr);
        glCompileShader(shader);
        return shader;
    }

    GLuint linkProgram(const std::vector<GLuint>& shaders)
    {
        const GLuint program = glCreateProgram();
        for (const GLuint shader : shaders)
        {
            glAttachShader(program, shader);
        }

        glLinkProgram(program);

        for (const GLuint shader : shaders)
        {
            glDeleteShader(shader);
        }

        return program;
    }
}

std::string readShaderFile(const std::string& filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << filePath << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint createShaderProgram(
    const std::string& vertexPath,
    const std::string& fragmentPath)
{
    return linkProgram({
        compileShader(GL_VERTEX_SHADER, readShaderFile(vertexPath)),
        compileShader(GL_FRAGMENT_SHADER, readShaderFile(fragmentPath))
    });
}

GLuint createShaderProgram(
    const std::string& vertexPath,
    const std::string& geometryPath,
    const std::string& fragmentPath)
{
    return linkProgram({
        compileShader(GL_VERTEX_SHADER, readShaderFile(vertexPath)),
        compileShader(GL_GEOMETRY_SHADER, readShaderFile(geometryPath)),
        compileShader(GL_FRAGMENT_SHADER, readShaderFile(fragmentPath))
    });
}
