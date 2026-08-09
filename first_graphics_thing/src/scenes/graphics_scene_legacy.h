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

        GLuint pointShadowMap;
        GLuint pointShadowFarPlane;
        GLuint pointShadowsEnabled;

        GLuint wallLightPos;
        GLuint wallLightColor;
        GLuint wallLightEnabled;
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

        GLuint pointShadowMap;
        GLuint pointShadowFarPlane;
        GLuint pointShadowsEnabled;

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

        GLuint wallLightPos;
        GLuint wallLightColor;
        GLuint wallLightEnabled;
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

    struct NormalMappingShaderUniforms
    {
        GLuint model;
        GLuint view;
        GLuint projection;

        GLuint lightPos;
        GLuint lightColor;
        GLuint viewPos;

        GLuint lightEnabled;

        GLuint diffuseMap;
        GLuint normalMap;
        GLuint depthMap;

        GLuint uvScale;
        GLuint normalStrength;
        GLuint heightScale;

        GLuint pointShadowMap;
        GLuint pointShadowFarPlane;
        GLuint pointShadowsEnabled;

        GLuint wallLightPos;
        GLuint wallLightColor;
        GLuint wallLightEnabled;
    };

    void drawLitObjects();
    void drawNormalDebugObjects();
    glm::mat4 createBackpackModelMatrix() const;
    glm::mat4 createOutdoorGroundModelMatrix() const;
    glm::mat4 createWallLampModelMatrix() const;
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

    glm::mat4 createAsteroidSystemModel() const;

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
        GLuint depthShaderProgram,
        const glm::mat4& lightSpaceMatrix
    );

    void createPointShadowMap();

    std::vector<glm::mat4>
        createPointShadowMatrices(
            const glm::vec3& lightPosition
        ) const;

    void renderPointShadowMap(
        const glm::vec3& lightPosition
    );

    void drawPointShadowCasters(
        GLuint depthShaderProgram
    );

    void configureModelPointShadows();

    void createNormalMappedPlane();

    void drawNormalMappedSurface(
        const glm::mat4& model,
        const glm::vec2& uvScale,
        GLuint diffuseTexture,
        GLuint normalTexture,
        GLuint depthTexture,
        float normalStrength,
        float heightScale,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& lightPos,
        const glm::vec3& lightColor
    );

    void drawNormalMappedRoomWalls(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& lightPos,
        const glm::vec3& lightColor
    );

    void drawNormalMappedFloorAndCeiling(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& lightPos,
        const glm::vec3& lightColor
    );

    void drawLampMarker(
        const glm::vec3& position,
        const glm::vec3& color,
        float scale = 0.15f
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
    Model* wallLamp = nullptr;

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

    static constexpr unsigned int POINT_SHADOW_WIDTH = 2048;
    static constexpr unsigned int POINT_SHADOW_HEIGHT = 2048;
    static constexpr float POINT_SHADOW_NEAR_PLANE = 0.1f;
    static constexpr float POINT_SHADOW_FAR_PLANE = 15.0f;

    GLuint pointDepthFBO = 0;
    GLuint pointDepthCubemap = 0;
    GLuint pointDepthShaderProgram = 0;

    GLuint directionalDepthFBO = 0;
    GLuint directionalDepthMap = 0;
    GLuint directionalDepthShaderProgram = 0;
    GLuint instancedDirectionalDepthShaderProgram = 0;

    int framebufferWidth = 0;
    int framebufferHeight = 0;

    GLuint multisampleFramebuffer = 0;
    GLuint multisampleColorTexture = 0;
    GLuint multisampleRBO = 0;

    GLuint screenQuadVAO = 0;
    GLuint screenQuadVBO = 0;

    GLuint screenShaderProgram = 0;
    GLuint screenTextureUniform = 0;

    GLuint normalMappingShaderProgram = 0;

    GLuint normalMappedPlaneVAO = 0;
    GLuint normalMappedPlaneVBO = 0;

    GLuint brickDiffuseTexture = 0;
    GLuint brickNormalTexture = 0;
    GLuint brickDepthTexture = 0;

    float parallaxHeightScale = 0.017f;

    GLuint floorDiffuseTexture = 0;
    GLuint floorNormalTexture = 0; 

    GLuint ceilingDiffuseTexture = 0;
    GLuint ceilingNormalTexture = 0;

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
    NormalMappingShaderUniforms normalMappingUniforms{};

    std::vector<glm::vec3> glassPanePositions{};
    std::vector<glm::mat4> asteroidMatrices;

    bool light1Enabled = true;
    bool light2Enabled = false;
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