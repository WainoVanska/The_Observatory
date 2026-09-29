#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace
{
std::unordered_map<GLuint, std::unordered_map<std::string, GLint>> uniformLocations;

const char* shaderTypeName(GLenum type)
{
    switch (type)
    {
    case GL_VERTEX_SHADER:
        return "vertex";
    case GL_GEOMETRY_SHADER:
        return "geometry";
    case GL_FRAGMENT_SHADER:
        return "fragment";
    default:
        return "unknown";
    }
}

GLuint compileShader(GLenum type, const std::string& source, const std::string& path)
{
    const GLuint shader = glCreateShader(type);
    const char* sourcePointer = source.c_str();
    glShaderSource(shader, 1, &sourcePointer, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Failed to compile " << shaderTypeName(type) << " shader: " << path << '\n' << log << '\n';
    }

    return shader;
}

GLuint linkProgram(const std::vector<GLuint>& shaders, const std::string& description)
{
    const GLuint program = glCreateProgram();
    for (const GLuint shader : shaders)
    {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cerr << "Failed to link shader program (" << description << "):" << '\n' << log << '\n';
    }

    for (const GLuint shader : shaders)
    {
        glDeleteShader(shader);
    }

    return program;
}
} // namespace

GLint getUniformLocation(GLuint program, const std::string& name)
{
    auto& programLocations = uniformLocations[program];
    const auto existing = programLocations.find(name);
    if (existing != programLocations.end())
    {
        return existing->second;
    }

    const GLint location = glGetUniformLocation(program, name.c_str());
    programLocations.emplace(name, location);
    return location;
}

void clearUniformLocationCache()
{
    uniformLocations.clear();
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

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
{
    return linkProgram(
        {
            compileShader(GL_VERTEX_SHADER, readShaderFile(vertexPath), vertexPath),
            compileShader(GL_FRAGMENT_SHADER, readShaderFile(fragmentPath), fragmentPath),
        },
        vertexPath + " + " + fragmentPath);
}

GLuint createShaderProgram(const std::string& vertexPath,
                           const std::string& geometryPath,
                           const std::string& fragmentPath)
{
    return linkProgram(
        {
            compileShader(GL_VERTEX_SHADER, readShaderFile(vertexPath), vertexPath),
            compileShader(GL_GEOMETRY_SHADER, readShaderFile(geometryPath), geometryPath),
            compileShader(GL_FRAGMENT_SHADER, readShaderFile(fragmentPath), fragmentPath),
        },
        vertexPath + " + " + geometryPath + " + " + fragmentPath);
}
