#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    std::string resolveShaderPath(const std::string& requestedPath)
    {
        constexpr const char* legacyScenePrefix = "src/week09/";
        constexpr const char* sceneShaderPrefix = "assets/shaders/scene/";

        if (requestedPath.rfind(legacyScenePrefix, 0) == 0)
        {
            return sceneShaderPrefix + requestedPath.substr(std::char_traits<char>::length(legacyScenePrefix));
        }

        if (requestedPath == "src/common/skybox_vertex_shader.glsl")
        {
            return "assets/shaders/skybox/skybox_vertex.glsl";
        }

        if (requestedPath == "src/common/skybox_fragment_shader.glsl")
        {
            return "assets/shaders/skybox/skybox_fragment.glsl";
        }

        return requestedPath;
    }

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
    const std::string resolvedPath = resolveShaderPath(filePath);
    std::ifstream file(resolvedPath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << resolvedPath << "\n";
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
