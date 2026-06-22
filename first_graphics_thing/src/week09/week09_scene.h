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
        const glm::vec3& cameraFront,
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

        GLuint flashlightPos;
        GLuint flashlightDir;
        GLuint flashlightEnabled;
        GLuint flashlightCutOff;
        GLuint flashlightOuterCutOff;
    };

    struct LampShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint color;
    };

    struct NormalDebugShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;
    };

    void drawLitObjects();
    void drawNormalDebugObjects();
    void drawLampMarker(const glm::vec3& position, const glm::vec3& color);

private:
    GLuint shaderProgram = 0;
    GLuint lampShaderProgram = 0;
    GLuint normalDebugShaderProgram = 0;

    GLuint containerTexture = 0;
    GLuint containerSpecularTexture = 0;

    CubeMesh cube{};
    SphereMesh sphere{};

    Material dullOrange{};
    Material shinyBlue{};
    Material matteGray{};
    Material texturedMaterial{};

    LightingShaderUniforms lightingUniforms{};
    LampShaderUniforms lampUniforms{};
    NormalDebugShaderUniforms normalDebugUniforms{};

    bool light1Enabled = true;
    bool light2Enabled = true;
    bool normalDebugEnabled = false;

    bool key1WasPressed = false;
    bool key2WasPressed = false;
    bool keyNWasPressed = false;
    bool flashlightEnabled = false;
    bool keyFWasPressed = false;
};