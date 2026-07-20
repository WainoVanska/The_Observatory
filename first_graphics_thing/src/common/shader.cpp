#include "shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

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
    const std::string& fragmentPath
)
{
    std::string vertexShaderCode = readShaderFile(vertexPath);
    std::string fragmentShaderCode = readShaderFile(fragmentPath);

    const char* vertexShaderSource = vertexShaderCode.c_str();
    const char* fragmentShaderSource = fragmentShaderCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint createShaderProgram(
    const std::string& vertexPath,
    const std::string& geometryPath,
    const std::string& fragmentPath
)
{
    const std::string vertexShaderCode =
        readShaderFile(vertexPath);

    const std::string geometryShaderCode =
        readShaderFile(geometryPath);

    const std::string fragmentShaderCode =
        readShaderFile(fragmentPath);

    const char* vertexShaderSource =
        vertexShaderCode.c_str();

    const char* geometryShaderSource =
        geometryShaderCode.c_str();

    const char* fragmentShaderSource =
        fragmentShaderCode.c_str();

    GLuint vertexShader =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);

    GLuint geometryShader =
        glCreateShader(GL_GEOMETRY_SHADER);

    glShaderSource(
        geometryShader,
        1,
        &geometryShaderSource,
        nullptr
    );

    glCompileShader(geometryShader);

    GLuint fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    GLuint shaderProgram =
        glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}