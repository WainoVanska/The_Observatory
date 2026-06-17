#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "../common/cube.h"
#include "../common/sphere.h"
#include "../common/material.h"

class Week09Scene
{
public:
    void init();

    void handleInput(GLFWwindow* window);

    void draw(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& movableLightPos
    );

    void cleanup();

private:
    struct LightingShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint viewPos;

        GLuint lightPos;
        GLuint lightColor;
        GLuint secondLightPos;
        GLuint secondLightColor;

        GLuint light1Enabled;
        GLuint light2Enabled;
    };

    struct LampShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint color;
    };

    void drawLitObjects();
    void drawLampMarker(const glm::vec3& position, const glm::vec3& color);

private:
    GLuint shaderProgram = 0;
    GLuint lampShaderProgram = 0;
    GLuint checkerboard = 0;

    CubeMesh cube{};
    SphereMesh sphere{};

    Material dullOrange{};
    Material shinyBlue{};
    Material matteGray{};

    LightingShaderUniforms lightingUniforms{};
    LampShaderUniforms lampUniforms{};

    bool light1Enabled = true;
    bool light2Enabled = true;

    bool key1WasPressed = false;
    bool key2WasPressed = false;
};