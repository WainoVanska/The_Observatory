#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <cstddef>
#include <vector>

#include <glm/glm.hpp>

#include "../engine/cube.h"
#include "../engine/free_camera.h"
#include "../engine/material.h"
#include "../engine/model.h"
#include "../engine/skybox.h"
#include "../engine/sphere.h"

class GraphicsScene
{
  public:
    void init(int width, int height, FreeCamera* camera);
    void handleInput(GLFWwindow* window);

    void draw(const glm::mat4& view,
              const glm::mat4& projection,
              const glm::vec3& cameraPos,
              const glm::vec3& movableLightPos,
              const glm::vec3& cameraFront);

    void cleanup();

  private:
    static constexpr std::size_t ROOM_POINT_LIGHT_COUNT = 4;
    static constexpr std::size_t POINT_SHADOW_FACE_COUNT = 6;

    struct RenderContext
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 cameraPosition;
        glm::vec3 cameraFront;
    };

    struct LightingState
    {
        glm::vec3 primaryPosition;
        glm::vec3 primaryColor;
        glm::vec3 secondaryPosition;
        glm::vec3 secondaryColor;
        bool primaryEnabled;
        bool secondaryEnabled;
        bool directionalEnabled;
        bool flashlightEnabled;
    };

    struct LightingShaderUniforms
    {
        GLint model = -1;
        GLint view = -1;
        GLint projection = -1;
        GLint viewPos = -1;
        GLint lightPos = -1;
        GLint lightColor = -1;
        GLint secondLightPos = -1;
        GLint secondLightColor = -1;
        GLint light1Enabled = -1;
        GLint light2Enabled = -1;
        GLint flashlightPos = -1;
        GLint flashlightDir = -1;
        GLint flashlightEnabled = -1;
        GLint flashlightCutOff = -1;
        GLint flashlightOuterCutOff = -1;
        GLint directionalLightEnabled = -1;
        GLint directionalLightDirection = -1;
        GLint directionalLightColor = -1;
        GLint lightSpaceMatrix = -1;
        GLint directionalShadowMap = -1;
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointShadowMaps{};
        GLint pointShadowFarPlane = -1;
        GLint pointShadowsEnabled = -1;
        GLint wallLightPos = -1;
        GLint wallLightColor = -1;
        GLint wallLightEnabled = -1;
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightPositions{};
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightColors{};
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightEnabled{};
    };

    struct ModelShaderUniforms
    {
        GLint model = -1;
        GLint view = -1;
        GLint projection = -1;
        GLint viewPos = -1;
        GLint lightPos = -1;
        GLint lightColor = -1;
        GLint secondLightPos = -1;
        GLint secondLightColor = -1;
        GLint light1Enabled = -1;
        GLint light2Enabled = -1;
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointShadowMaps{};
        GLint pointShadowFarPlane = -1;
        GLint pointShadowsEnabled = -1;
        GLint directionalLightEnabled = -1;
        GLint directionalLightDirection = -1;
        GLint directionalLightColor = -1;
        GLint flashlightPos = -1;
        GLint flashlightDir = -1;
        GLint flashlightEnabled = -1;
        GLint flashlightCutOff = -1;
        GLint flashlightOuterCutOff = -1;
        GLint skybox = -1;
        GLint glassBackpackEnabled = -1;
        GLint materialShininess = -1;
        GLint wallLightPos = -1;
        GLint wallLightColor = -1;
        GLint wallLightEnabled = -1;
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightPositions{};
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightColors{};
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightEnabled{};
    };

    struct InstancedModelShaderUniforms
    {
        GLint view = -1;
        GLint projection = -1;
        GLint viewPos = -1;
        GLint lightPos = -1;
        GLint lightColor = -1;
        GLint secondLightPos = -1;
        GLint secondLightColor = -1;
        GLint light1Enabled = -1;
        GLint light2Enabled = -1;
        GLint directionalLightEnabled = -1;
        GLint directionalLightDirection = -1;
        GLint directionalLightColor = -1;
        GLint flashlightPos = -1;
        GLint flashlightDir = -1;
        GLint flashlightEnabled = -1;
        GLint flashlightCutOff = -1;
        GLint flashlightOuterCutOff = -1;
        GLint lightSpaceMatrix = -1;
        GLint directionalShadowMap = -1;
        GLint systemModel = -1;
        GLint objectColor = -1;
        GLint objectSpecularColor = -1;
        GLint shininess = -1;
    };

    struct LampShaderUniforms
    {
        GLint model = -1;
        GLint view = -1;
        GLint projection = -1;
        GLint color = -1;
    };

    struct MatrixShaderUniforms
    {
        GLint model = -1;
        GLint view = -1;
        GLint projection = -1;
    };

    struct DepthDebugShaderUniforms : MatrixShaderUniforms
    {
        GLint nearPlane = -1;
        GLint farPlane = -1;
    };

    struct GlassShaderUniforms : MatrixShaderUniforms
    {
        GLint texture = -1;
    };

    struct NormalMappingShaderUniforms
    {
        GLint model = -1;
        GLint view = -1;
        GLint projection = -1;
        GLint lightPos = -1;
        GLint lightColor = -1;
        GLint viewPos = -1;
        GLint lightEnabled = -1;
        GLint diffuseMap = -1;
        GLint normalMap = -1;
        GLint depthMap = -1;
        GLint uvScale = -1;
        GLint normalStrength = -1;
        GLint heightScale = -1;
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointShadowMaps{};
        GLint pointShadowFarPlane = -1;
        GLint pointShadowsEnabled = -1;
        GLint wallLightPos = -1;
        GLint wallLightColor = -1;
        GLint wallLightEnabled = -1;
        GLint flashlightPos = -1;
        GLint flashlightDir = -1;
        GLint flashlightEnabled = -1;
        GLint flashlightCutOff = -1;
        GLint flashlightOuterCutOff = -1;
        GLint normalDebugEnabled = -1;
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightPositions{};
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightColors{};
        std::array<GLint, ROOM_POINT_LIGHT_COUNT> pointLightEnabled{};
    };

    struct ScreenShaderUniforms
    {
        GLint screenTexture = -1;
        GLint bloomTexture = -1;
        GLint depthTexture = -1;
        GLint blurEnabled = -1;
        GLint gammaCorrectionEnabled = -1;
        GLint hdrEnabled = -1;
        GLint exposure = -1;
        GLint bloomEnabled = -1;
        GLint bloomStrength = -1;
        GLint depthDebugEnabled = -1;
        GLint nearPlane = -1;
        GLint farPlane = -1;
    };

    struct BrightPassShaderUniforms
    {
        GLint sceneTexture = -1;
        GLint brightnessThreshold = -1;
    };

    struct GaussianBlurShaderUniforms
    {
        GLint image = -1;
        GLint horizontal = -1;
    };

    struct DirectionalDepthShaderUniforms
    {
        GLint lightSpaceMatrix = -1;
        GLint model = -1;
    };

    struct InstancedDirectionalDepthShaderUniforms
    {
        GLint lightSpaceMatrix = -1;
        GLint systemModel = -1;
    };

    struct PointDepthShaderUniforms
    {
        std::array<GLint, POINT_SHADOW_FACE_COUNT> shadowMatrices{};
        GLint lightPosition = -1;
        GLint farPlane = -1;
        GLint model = -1;
    };

    void drawLitObjects();
    void drawNormalDebugObjects();
    void drawDepthDebugObjects();
    void drawLampMarker(const glm::vec3& position, const glm::vec3& color, float scale = 0.15f);

    glm::mat4 createBackpackModelMatrix() const;
    glm::mat4 createBackpackOutlineModelMatrix() const;
    glm::mat4 createOutdoorGroundModelMatrix() const;
    glm::mat4 createAsteroidSystemModel() const;

    void drawBackpack(const RenderContext& context, const LightingState& lighting);
    void drawBackpackNormalDebug(const glm::mat4& view, const glm::mat4& projection);
    void drawBackpackDepthDebug(const glm::mat4& view, const glm::mat4& projection);
    void drawBackpackOutline(const glm::mat4& view, const glm::mat4& projection);

    void createGlassPaneMesh();
    void drawGlassPanes(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);

    void createFramebuffer(int width, int height);
    void createBloomFramebuffer(int width, int height);
    void createPingPongFramebuffers(int width, int height);
    void createScreenQuad();
    void extractBrightAreas();
    void blurBrightAreas();
    void drawScreenQuad();

    void createAsteroidInstances();
    void drawMiniPlanetSystem(const RenderContext& context, const LightingState& lighting);

    void createDirectionalShadowMap();
    glm::mat4 createDirectionalLightSpaceMatrix(const glm::vec3& sunDirection) const;
    void renderDirectionalShadowMap(const glm::mat4& lightSpaceMatrix);
    void drawDirectionalShadowCasters(GLuint depthShaderProgram, const glm::mat4& lightSpaceMatrix);

    void createPointShadowMaps();
    std::vector<glm::mat4> createPointShadowMatrices(const glm::vec3& lightPosition) const;
    void renderPointShadowMap(const glm::vec3& lightPosition, GLuint targetCubemap);
    void drawPointShadowCasters(GLuint depthShaderProgram);
    void configureModelPointShadows();

    void createNormalMappedPlane();
    void drawNormalMappedSurface(const glm::mat4& model,
                                 const glm::vec2& uvScale,
                                 GLuint diffuseTexture,
                                 GLuint normalTexture,
                                 GLuint depthTexture,
                                 float normalStrength,
                                 float heightScale,
                                 const RenderContext& context,
                                 const LightingState& lighting);
    void drawNormalMappedRoomWalls(const RenderContext& context, const LightingState& lighting);
    void drawNormalMappedFloorAndCeiling(const RenderContext& context, const LightingState& lighting);

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
    GLuint directionalDepthShaderProgram = 0;
    GLuint instancedDirectionalDepthShaderProgram = 0;
    GLuint pointDepthShaderProgram = 0;
    GLuint normalMappingShaderProgram = 0;
    GLuint screenShaderProgram = 0;
    GLuint brightPassShaderProgram = 0;
    GLuint gaussianBlurShaderProgram = 0;

    GLuint asteroidInstanceVBO = 0;

    Model* backpack = nullptr;
    SkyBox* skybox = nullptr;
    Model* planet = nullptr;
    Model* rock = nullptr;

    GLuint containerTexture = 0;
    GLuint containerSpecularTexture = 0;
    GLuint glassTexture = 0;
    GLuint brickDiffuseTexture = 0;
    GLuint brickNormalTexture = 0;
    GLuint brickDepthTexture = 0;
    GLuint floorDiffuseTexture = 0;
    GLuint floorNormalTexture = 0;
    GLuint ceilingDiffuseTexture = 0;
    GLuint ceilingNormalTexture = 0;

    GLuint glassVAO = 0;
    GLuint glassVBO = 0;
    GLuint normalMappedPlaneVAO = 0;
    GLuint normalMappedPlaneVBO = 0;
    GLuint screenQuadVAO = 0;
    GLuint screenQuadVBO = 0;

    GLuint framebuffer = 0;
    GLuint framebufferTexture = 0;
    GLuint framebufferDepthTexture = 0;
    GLuint multisampleFramebuffer = 0;
    GLuint multisampleColorTexture = 0;
    GLuint multisampleRBO = 0;
    GLuint brightPassFBO = 0;
    GLuint brightTexture = 0;
    std::array<GLuint, 2> pingPongFBO{};
    std::array<GLuint, 2> pingPongTextures{};
    GLuint blurredBloomTexture = 0;

    GLuint directionalDepthFBO = 0;
    GLuint directionalDepthMap = 0;
    GLuint pointDepthFBO = 0;
    std::array<GLuint, ROOM_POINT_LIGHT_COUNT> pointDepthCubemaps{};
    std::array<bool, ROOM_POINT_LIGHT_COUNT> fixedPointShadowMapsValid{};

    int framebufferWidth = 0;
    int framebufferHeight = 0;

    static constexpr int MSAA_SAMPLES = 4;
    static constexpr unsigned int ASTEROID_COUNT = 10000;
    static constexpr unsigned int SHADOW_WIDTH = 2048;
    static constexpr unsigned int SHADOW_HEIGHT = 2048;
    static constexpr unsigned int POINT_SHADOW_WIDTH = 1024;
    static constexpr unsigned int POINT_SHADOW_HEIGHT = 1024;
    static constexpr float POINT_SHADOW_NEAR_PLANE = 0.1f;
    static constexpr float POINT_SHADOW_FAR_PLANE = 15.0f;

    bool bloomEnabled = true;
    float bloomStrength = 0.55f;
    float parallaxHeightScale = 0.017f;

    CubeMesh cube{};
    SphereMesh sphere{};

    Material shinyBlue{};
    Material texturedMaterial{};
    Material outdoorGroundMaterial{};

    LightingShaderUniforms lightingUniforms{};
    ModelShaderUniforms modelUniforms{};
    InstancedModelShaderUniforms instancedModelUniforms{};
    LampShaderUniforms lampUniforms{};
    MatrixShaderUniforms normalDebugUniforms{};
    MatrixShaderUniforms modelNormalDebugUniforms{};
    DepthDebugShaderUniforms depthDebugUniforms{};
    DepthDebugShaderUniforms modelDepthDebugUniforms{};
    MatrixShaderUniforms modelOutlineUniforms{};
    GlassShaderUniforms glassUniforms{};
    NormalMappingShaderUniforms normalMappingUniforms{};
    ScreenShaderUniforms screenUniforms{};
    BrightPassShaderUniforms brightPassUniforms{};
    GaussianBlurShaderUniforms gaussianBlurUniforms{};
    DirectionalDepthShaderUniforms directionalDepthUniforms{};
    InstancedDirectionalDepthShaderUniforms instancedDirectionalDepthUniforms{};
    PointDepthShaderUniforms pointDepthUniforms{};

    struct GlassSurface
    {
        glm::vec3 position;
        glm::vec3 scale;
        float yRotation;
        bool optional;
    };

    std::vector<GlassSurface> glassSurfaces{};
    std::vector<glm::mat4> asteroidMatrices;

    bool movableLightModeEnabled = false;
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
