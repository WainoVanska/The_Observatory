#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "../common/cube.h"
#include "../common/sphere.h"
#include "../common/material.h"
#include "../common/model.h"

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

    struct ModelNormalDebugShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;
    };

    struct DepthDebugShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint nearPlane;
        GLuint farPlane;
    };

    struct ModelDepthDebugShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint nearPlane;
        GLuint farPlane;
    };

    struct ModelShaderUniforms
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

    struct ModelOutlineShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;
    };

    void drawLitObjects();
    void drawNormalDebugObjects();
    glm::mat4 createBackpackModelMatrix() const;
    void drawLampMarker(const glm::vec3& position, const glm::vec3& color);
    void drawBackpack(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& cameraFront,
        const glm::vec3& lightPos,
        const glm::vec3& secondLightPos,
        const glm::vec3& secondLightColor
    );
    void drawBackpackNormalDebug(
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawDepthDebugObjects();

    void drawBackpackDepthDebug(
        const glm::mat4& view,
        const glm::mat4& projection
    );

    glm::mat4 createBackpackOutlineModelMatrix() const;

    void drawBackpackOutline(
        const glm::mat4& view,
        const glm::mat4& projection
    );

private:
    GLuint shaderProgram = 0;
    GLuint lampShaderProgram = 0;
    GLuint normalDebugShaderProgram = 0;

    GLuint modelShaderProgram = 0;
    GLuint modelNormalDebugShaderProgram = 0;

    GLuint depthDebugShaderProgram = 0;
    GLuint modelDepthDebugShaderProgram = 0;

    GLuint modelOutlineShaderProgram = 0;

    Model* backpack = nullptr;

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
    ModelNormalDebugShaderUniforms modelNormalDebugUniforms{};
    ModelShaderUniforms modelUniforms{};
    DepthDebugShaderUniforms depthDebugUniforms{};
    ModelDepthDebugShaderUniforms modelDepthDebugUniforms{};
    ModelOutlineShaderUniforms modelOutlineUniforms{};


    bool light1Enabled = true;
    bool light2Enabled = true;
    bool normalDebugEnabled = false;
    bool depthDebugEnabled = false;

    bool keyVWasPressed = false;
    bool key1WasPressed = false;
    bool key2WasPressed = false;
    bool keyNWasPressed = false;
    bool flashlightEnabled = false;
    bool keyFWasPressed = false;
    bool outlineEnabled = false;
    bool keyPWasPressed = false;
};