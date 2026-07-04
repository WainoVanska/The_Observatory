#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "../common/cube.h"
#include "../common/sphere.h"
#include "../common/material.h"
#include "../common/model.h"

class Week09Scene
{
public:
    void init(int width, int height);

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

    struct GlassShaderUniforms
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

    void createGlassPaneMesh();

    void drawGlassPanes(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos
    );

    void createFramebuffer(int width, int height);
    void createScreenQuad();
    void drawScreenQuad();

private:
    GLuint shaderProgram = 0;
    GLuint lampShaderProgram = 0;
    GLuint normalDebugShaderProgram = 0;

    GLuint modelShaderProgram = 0;
    GLuint modelNormalDebugShaderProgram = 0;

    GLuint depthDebugShaderProgram = 0;
    GLuint modelDepthDebugShaderProgram = 0;

    GLuint modelOutlineShaderProgram = 0;

    GLuint glassShaderProgram = 0;

    Model* backpack = nullptr;

    GLuint containerTexture = 0;
    GLuint containerSpecularTexture = 0;
    GLuint glassTexture = 0;

    GLuint glassVAO = 0;
    GLuint glassVBO = 0;

    GLuint framebuffer = 0;
    GLuint framebufferTexture = 0;
    GLuint framebufferRBO = 0;

    GLuint screenQuadVAO = 0;
    GLuint screenQuadVBO = 0;

    GLuint screenShaderProgram = 0;
    GLuint screenTextureUniform = 0;

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
    GlassShaderUniforms glassUniforms{};

    std::vector<glm::vec3> glassPanePositions{};

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
    bool glassEnabled = false;
    bool keyGWasPressed = false;

    bool blurEnabled = false;
    bool keyBWasPressed = false;
};