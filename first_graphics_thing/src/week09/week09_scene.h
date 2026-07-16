#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "../common/cube.h"
#include "../common/sphere.h"
#include "../common/material.h"
#include "../common/model.h"

#include "../common/skybox.h"
#include "../common/free_camera.h"

class Week09Scene
{
public:
    void init(int width, int height, FreeCamera* camera);

    void handleInput(GLFWwindow* window);

    void draw(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& movableLightPos,
        const glm::vec3& cameraFront
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

        GLuint directionalLightEnabled;
        GLuint directionalLightDirection;
        GLuint directionalLightColor;

        GLuint lightSpaceMatrix;
        GLuint directionalShadowMap;
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

        GLuint directionalLightEnabled;
        GLuint directionalLightDirection;
        GLuint directionalLightColor;

        GLuint flashlightPos;
        GLuint flashlightDir;
        GLuint flashlightEnabled;
        GLuint flashlightCutOff;
        GLuint flashlightOuterCutOff;

        GLuint skybox;
        GLuint glassBackpackEnabled;
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
    glm::mat4 createOutdoorGroundModelMatrix() const;
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

    void createAsteroidInstances();

    void drawMiniPlanetSystem(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& cameraFront,
        const glm::vec3& lightPos,
        const glm::vec3& secondLightPos,
        const glm::vec3& secondLightColor
    );

    void createDirectionalShadowMap();

    glm::mat4 createDirectionalLightSpaceMatrix(
        const glm::vec3& sunDirection
    ) const;

    void renderDirectionalShadowMap(
        const glm::mat4& lightSpaceMatrix
    );

    void drawDirectionalShadowCasters(
        GLuint depthShaderProgram
    );

    GLuint shaderProgram = 0;
    GLuint lampShaderProgram = 0;
    GLuint normalDebugShaderProgram = 0;
    GLuint modelShaderProgram = 0;
    GLuint modelNormalDebugShaderProgram = 0;
    GLuint depthDebugShaderProgram = 0;
    GLuint modelDepthDebugShaderProgram = 0;
    GLuint modelOutlineShaderProgram = 0;
    GLuint glassShaderProgram = 0;
    GLuint instancedModelShaderProgram = 0;

    GLuint asteroidInstanceVBO = 0;

    Model* backpack = nullptr;
    SkyBox* skybox = nullptr;
    Model* planet = nullptr;
    Model* rock = nullptr;

    GLuint containerTexture = 0;
    GLuint containerSpecularTexture = 0;
    GLuint glassTexture = 0;

    GLuint glassVAO = 0;
    GLuint glassVBO = 0;

    GLuint framebuffer = 0;
    GLuint framebufferTexture = 0;
    GLuint framebufferRBO = 0;

    static constexpr int MSAA_SAMPLES = 4;
    static constexpr unsigned int ASTEROID_COUNT = 10000;

    static constexpr unsigned int SHADOW_WIDTH = 2048;
    static constexpr unsigned int SHADOW_HEIGHT = 2048;

    GLuint directionalDepthFBO = 0;
    GLuint directionalDepthMap = 0;
    GLuint directionalDepthShaderProgram = 0;

    int framebufferWidth = 0;
    int framebufferHeight = 0;

    GLuint multisampleFramebuffer = 0;
    GLuint multisampleColorTexture = 0;
    GLuint multisampleRBO = 0;

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
    Material outdoorGroundMaterial{};

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
    std::vector<glm::mat4> asteroidMatrices;

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
    bool outdoorModeEnabled = false;
    bool keyMWasPressed = false;
    bool glassBackpackEnabled = false;
    bool keyHWasPressed = false;
};