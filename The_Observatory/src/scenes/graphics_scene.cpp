#include "graphics_scene.h"
#include "graphics_scene_support.h"

#include "../engine/shader.h"
#include "../engine/texture.h"

#include <array>
#include <cstddef>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace graphics_scene_detail;

void GraphicsScene::init(int width, int height, FreeCamera* camera)
{
    shaderProgram = createShaderProgram("assets/shaders/scene/vertex.glsl", "assets/shaders/scene/fragment.glsl");
    lampShaderProgram = createShaderProgram("assets/shaders/scene/lamp_vertex.glsl", "assets/shaders/scene/lamp_fragment.glsl");
    normalDebugShaderProgram = createShaderProgram("assets/shaders/scene/vertex.glsl", "assets/shaders/scene/normal_debug_fragment.glsl");
    modelShaderProgram = createShaderProgram("assets/shaders/scene/model_vertex.glsl", "assets/shaders/scene/model_fragment.glsl");
    modelNormalDebugShaderProgram = createShaderProgram("assets/shaders/scene/model_vertex.glsl", "assets/shaders/scene/normal_debug_fragment.glsl");
    depthDebugShaderProgram = createShaderProgram("assets/shaders/scene/vertex.glsl", "assets/shaders/scene/depth_debug_fragment.glsl");
    modelDepthDebugShaderProgram = createShaderProgram("assets/shaders/scene/model_vertex.glsl", "assets/shaders/scene/depth_debug_fragment.glsl");
    modelOutlineShaderProgram = createShaderProgram("assets/shaders/scene/model_vertex.glsl", "assets/shaders/scene/single_color_fragment.glsl");
    glassShaderProgram = createShaderProgram("assets/shaders/scene/glass_vertex.glsl", "assets/shaders/scene/glass_fragment.glsl");
    screenShaderProgram = createShaderProgram("assets/shaders/scene/screen_vertex.glsl", "assets/shaders/scene/screen_fragment.glsl");
    brightPassShaderProgram = createShaderProgram("assets/shaders/scene/screen_vertex.glsl", "assets/shaders/scene/bright_pass_fragment.glsl");
    gaussianBlurShaderProgram = createShaderProgram("assets/shaders/scene/screen_vertex.glsl", "assets/shaders/scene/gaussian_blur_fragment.glsl");
    instancedModelShaderProgram = createShaderProgram("assets/shaders/scene/instanced_model_vertex.glsl", "assets/shaders/scene/instanced_rock_fragment.glsl");
    directionalDepthShaderProgram = createShaderProgram("assets/shaders/scene/directional_depth_vertex.glsl", "assets/shaders/scene/directional_depth_fragment.glsl");
    instancedDirectionalDepthShaderProgram = createShaderProgram("assets/shaders/scene/instanced_directional_depth_vertex.glsl", "assets/shaders/scene/directional_depth_fragment.glsl");
    pointDepthShaderProgram = createShaderProgram("assets/shaders/scene/point_depth_vertex.glsl", "assets/shaders/scene/point_depth_geometry.glsl", "assets/shaders/scene/point_depth_fragment.glsl");
    normalMappingShaderProgram = createShaderProgram("assets/shaders/scene/normal_mapping_vertex.glsl", "assets/shaders/scene/normal_mapping_fragment.glsl");

    containerTexture = loadTexture("assets/textures/container.png", true);
    brickDiffuseTexture = loadTexture("assets/textures/brickwall.jpg", true);
    floorDiffuseTexture = loadTexture("assets/textures/wood_floor_color.jpg", true);
    ceilingDiffuseTexture = loadTexture("assets/textures/plaster_color.jpg", true);
    containerSpecularTexture = loadTexture("assets/textures/container_specular.png", false);
    brickNormalTexture = loadTexture("assets/textures/brickwall_normal.jpg", false);
    brickDepthTexture = loadTexture("assets/textures/brickwall_disp.jpg", false);
    floorNormalTexture = loadTexture("assets/textures/wood_floor_normal.jpg", false);
    ceilingNormalTexture = loadTexture("assets/textures/plaster_normal.jpg", false);
    glassTexture = loadTransparentTexture("assets/textures/observatory_window.png", true);

    backpack = new Model("assets/models/backpack/backpack.obj");
    planet = new Model("assets/models/planet/planet.obj");
    rock = new Model("assets/models/rock/rock.obj");
    cube = createCubeMesh();
    sphere = createSphereMesh();

    createAsteroidInstances();
    createGlassPaneMesh();
    createFramebuffer(width, height);
    createBloomFramebuffer(width, height);
    createPingPongFramebuffers(width, height);
    createScreenQuad();
    createDirectionalShadowMap();
    createPointShadowMaps();
    createNormalMappedPlane();

    glassSurfaces = {{glm::vec3(0.0f, 1.0f, 2.94f), glm::vec3(6.0f, 4.0f, 1.0f), 180.0f, false}, // glass wall
                     {glm::vec3(-1.8f, 0.0f, -1.8f), glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true},
                     {glm::vec3(0.0f, 0.0f, -1.8f), glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true},
                     {glm::vec3(1.8f, 0.0f, -1.8f), glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true},
                     {glm::vec3(-1.2f, 0.0f, 0.8f), glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true},
                     {glm::vec3(1.2f, 0.0f, 0.8f), glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true}};

    outdoorGroundMaterial.diffuseColor = glm::vec3(0.18f, 0.42f, 0.16f);
    outdoorGroundMaterial.specularColor = glm::vec3(0.05f);
    outdoorGroundMaterial.shininess = 8.0f;
    outdoorGroundMaterial.useDiffuseMap = false;
    outdoorGroundMaterial.useSpecularMap = false;
    shinyBlue = createShinyBlueMaterial();
    texturedMaterial = createTexturedMaterial();

    skybox = new SkyBox(camera);
    if (!skybox->Init("assets/textures/space", "px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png"))
    {
        std::cout << "Skybox failed to load\n";
    }

    const auto location = [](GLuint program, const char* name)
    {
        return getUniformLocation(program, name);
    };

    lightingUniforms.model = location(shaderProgram, "model");
    lightingUniforms.view = location(shaderProgram, "view");
    lightingUniforms.projection = location(shaderProgram, "projection");
    lightingUniforms.viewPos = location(shaderProgram, "viewPos");
    lightingUniforms.lightPos = location(shaderProgram, "lightPos");
    lightingUniforms.lightColor = location(shaderProgram, "lightColor");
    lightingUniforms.secondLightPos = location(shaderProgram, "secondLightPos");
    lightingUniforms.secondLightColor = location(shaderProgram, "secondLightColor");
    lightingUniforms.light1Enabled = location(shaderProgram, "light1Enabled");
    lightingUniforms.light2Enabled = location(shaderProgram, "light2Enabled");
    lightingUniforms.flashlightPos = location(shaderProgram, "flashlightPos");
    lightingUniforms.flashlightDir = location(shaderProgram, "flashlightDir");
    lightingUniforms.flashlightEnabled = location(shaderProgram, "flashlightEnabled");
    lightingUniforms.flashlightCutOff = location(shaderProgram, "flashlightCutOff");
    lightingUniforms.flashlightOuterCutOff = location(shaderProgram, "flashlightOuterCutOff");
    lightingUniforms.directionalLightEnabled = location(shaderProgram, "directionalLightEnabled");
    lightingUniforms.directionalLightDirection = location(shaderProgram, "directionalLightDirection");
    lightingUniforms.directionalLightColor = location(shaderProgram, "directionalLightColor");
    lightingUniforms.lightSpaceMatrix = location(shaderProgram, "lightSpaceMatrix");
    lightingUniforms.directionalShadowMap = location(shaderProgram, "directionalShadowMap");
    lightingUniforms.pointShadowFarPlane = location(shaderProgram, "pointShadowFarPlane");
    lightingUniforms.pointShadowsEnabled = location(shaderProgram, "pointShadowsEnabled");
    lightingUniforms.wallLightPos = location(shaderProgram, "wallLightPos");
    lightingUniforms.wallLightColor = location(shaderProgram, "wallLightColor");
    lightingUniforms.wallLightEnabled = location(shaderProgram, "wallLightEnabled");

    lampUniforms.model = location(lampShaderProgram, "model");
    lampUniforms.view = location(lampShaderProgram, "view");
    lampUniforms.projection = location(lampShaderProgram, "projection");
    lampUniforms.color = location(lampShaderProgram, "lampColor");

    normalDebugUniforms.model = location(normalDebugShaderProgram, "model");
    normalDebugUniforms.view = location(normalDebugShaderProgram, "view");
    normalDebugUniforms.projection = location(normalDebugShaderProgram, "projection");
    modelNormalDebugUniforms.model = location(modelNormalDebugShaderProgram, "model");
    modelNormalDebugUniforms.view = location(modelNormalDebugShaderProgram, "view");
    modelNormalDebugUniforms.projection = location(modelNormalDebugShaderProgram, "projection");

    depthDebugUniforms.model = location(depthDebugShaderProgram, "model");
    depthDebugUniforms.view = location(depthDebugShaderProgram, "view");
    depthDebugUniforms.projection = location(depthDebugShaderProgram, "projection");
    depthDebugUniforms.nearPlane = location(depthDebugShaderProgram, "nearPlane");
    depthDebugUniforms.farPlane = location(depthDebugShaderProgram, "farPlane");
    modelDepthDebugUniforms.model = location(modelDepthDebugShaderProgram, "model");
    modelDepthDebugUniforms.view = location(modelDepthDebugShaderProgram, "view");
    modelDepthDebugUniforms.projection = location(modelDepthDebugShaderProgram, "projection");
    modelDepthDebugUniforms.nearPlane = location(modelDepthDebugShaderProgram, "nearPlane");
    modelDepthDebugUniforms.farPlane = location(modelDepthDebugShaderProgram, "farPlane");

    modelUniforms.model = location(modelShaderProgram, "model");
    modelUniforms.view = location(modelShaderProgram, "view");
    modelUniforms.projection = location(modelShaderProgram, "projection");
    modelUniforms.viewPos = location(modelShaderProgram, "viewPos");
    modelUniforms.lightPos = location(modelShaderProgram, "lightPos");
    modelUniforms.lightColor = location(modelShaderProgram, "lightColor");
    modelUniforms.secondLightPos = location(modelShaderProgram, "secondLightPos");
    modelUniforms.secondLightColor = location(modelShaderProgram, "secondLightColor");
    modelUniforms.light1Enabled = location(modelShaderProgram, "light1Enabled");
    modelUniforms.light2Enabled = location(modelShaderProgram, "light2Enabled");
    modelUniforms.pointShadowFarPlane = location(modelShaderProgram, "pointShadowFarPlane");
    modelUniforms.pointShadowsEnabled = location(modelShaderProgram, "pointShadowsEnabled");
    modelUniforms.directionalLightEnabled = location(modelShaderProgram, "directionalLightEnabled");
    modelUniforms.directionalLightDirection = location(modelShaderProgram, "directionalLightDirection");
    modelUniforms.directionalLightColor = location(modelShaderProgram, "directionalLightColor");
    modelUniforms.flashlightPos = location(modelShaderProgram, "flashlightPos");
    modelUniforms.flashlightDir = location(modelShaderProgram, "flashlightDir");
    modelUniforms.flashlightEnabled = location(modelShaderProgram, "flashlightEnabled");
    modelUniforms.flashlightCutOff = location(modelShaderProgram, "flashlightCutOff");
    modelUniforms.flashlightOuterCutOff = location(modelShaderProgram, "flashlightOuterCutOff");
    modelUniforms.skybox = location(modelShaderProgram, "skybox");
    modelUniforms.glassBackpackEnabled = location(modelShaderProgram, "glassBackpackEnabled");
    modelUniforms.materialShininess = location(modelShaderProgram, "material.shininess");
    modelUniforms.wallLightPos = location(modelShaderProgram, "wallLightPos");
    modelUniforms.wallLightColor = location(modelShaderProgram, "wallLightColor");
    modelUniforms.wallLightEnabled = location(modelShaderProgram, "wallLightEnabled");

    modelOutlineUniforms.model = location(modelOutlineShaderProgram, "model");
    modelOutlineUniforms.view = location(modelOutlineShaderProgram, "view");
    modelOutlineUniforms.projection = location(modelOutlineShaderProgram, "projection");
    glassUniforms.model = location(glassShaderProgram, "model");
    glassUniforms.view = location(glassShaderProgram, "view");
    glassUniforms.projection = location(glassShaderProgram, "projection");
    glassUniforms.texture = location(glassShaderProgram, "glassTexture");

    instancedModelUniforms.view = location(instancedModelShaderProgram, "view");
    instancedModelUniforms.projection = location(instancedModelShaderProgram, "projection");
    instancedModelUniforms.viewPos = location(instancedModelShaderProgram, "viewPos");
    instancedModelUniforms.lightPos = location(instancedModelShaderProgram, "lightPos");
    instancedModelUniforms.lightColor = location(instancedModelShaderProgram, "lightColor");
    instancedModelUniforms.secondLightPos = location(instancedModelShaderProgram, "secondLightPos");
    instancedModelUniforms.secondLightColor = location(instancedModelShaderProgram, "secondLightColor");
    instancedModelUniforms.light1Enabled = location(instancedModelShaderProgram, "light1Enabled");
    instancedModelUniforms.light2Enabled = location(instancedModelShaderProgram, "light2Enabled");
    instancedModelUniforms.directionalLightEnabled = location(instancedModelShaderProgram, "directionalLightEnabled");
    instancedModelUniforms.directionalLightDirection =
        location(instancedModelShaderProgram, "directionalLightDirection");
    instancedModelUniforms.directionalLightColor = location(instancedModelShaderProgram, "directionalLightColor");
    instancedModelUniforms.flashlightPos = location(instancedModelShaderProgram, "flashlightPos");
    instancedModelUniforms.flashlightDir = location(instancedModelShaderProgram, "flashlightDir");
    instancedModelUniforms.flashlightEnabled = location(instancedModelShaderProgram, "flashlightEnabled");
    instancedModelUniforms.flashlightCutOff = location(instancedModelShaderProgram, "flashlightCutOff");
    instancedModelUniforms.flashlightOuterCutOff = location(instancedModelShaderProgram, "flashlightOuterCutOff");
    instancedModelUniforms.lightSpaceMatrix = location(instancedModelShaderProgram, "lightSpaceMatrix");
    instancedModelUniforms.directionalShadowMap = location(instancedModelShaderProgram, "directionalShadowMap");
    instancedModelUniforms.systemModel = location(instancedModelShaderProgram, "systemModel");
    instancedModelUniforms.objectColor = location(instancedModelShaderProgram, "objectColor");
    instancedModelUniforms.objectSpecularColor = location(instancedModelShaderProgram, "objectSpecularColor");
    instancedModelUniforms.shininess = location(instancedModelShaderProgram, "shininess");

    normalMappingUniforms.model = location(normalMappingShaderProgram, "model");
    normalMappingUniforms.view = location(normalMappingShaderProgram, "view");
    normalMappingUniforms.projection = location(normalMappingShaderProgram, "projection");
    normalMappingUniforms.lightPos = location(normalMappingShaderProgram, "lightPos");
    normalMappingUniforms.lightColor = location(normalMappingShaderProgram, "lightColor");
    normalMappingUniforms.viewPos = location(normalMappingShaderProgram, "viewPos");
    normalMappingUniforms.lightEnabled = location(normalMappingShaderProgram, "lightEnabled");
    normalMappingUniforms.diffuseMap = location(normalMappingShaderProgram, "diffuseMap");
    normalMappingUniforms.normalMap = location(normalMappingShaderProgram, "normalMap");
    normalMappingUniforms.depthMap = location(normalMappingShaderProgram, "depthMap");
    normalMappingUniforms.heightScale = location(normalMappingShaderProgram, "heightScale");
    normalMappingUniforms.uvScale = location(normalMappingShaderProgram, "uvScale");
    normalMappingUniforms.normalStrength = location(normalMappingShaderProgram, "normalStrength");
    normalMappingUniforms.pointShadowFarPlane = location(normalMappingShaderProgram, "pointShadowFarPlane");
    normalMappingUniforms.pointShadowsEnabled = location(normalMappingShaderProgram, "pointShadowsEnabled");
    normalMappingUniforms.wallLightPos = location(normalMappingShaderProgram, "wallLightPos");
    normalMappingUniforms.wallLightColor = location(normalMappingShaderProgram, "wallLightColor");
    normalMappingUniforms.wallLightEnabled = location(normalMappingShaderProgram, "wallLightEnabled");
    normalMappingUniforms.flashlightPos = location(normalMappingShaderProgram, "flashlightPos");
    normalMappingUniforms.flashlightDir = location(normalMappingShaderProgram, "flashlightDir");
    normalMappingUniforms.flashlightEnabled = location(normalMappingShaderProgram, "flashlightEnabled");
    normalMappingUniforms.flashlightCutOff = location(normalMappingShaderProgram, "flashlightCutOff");
    normalMappingUniforms.flashlightOuterCutOff = location(normalMappingShaderProgram, "flashlightOuterCutOff");
    normalMappingUniforms.normalDebugEnabled = location(normalMappingShaderProgram, "normalDebugEnabled");

    screenUniforms.screenTexture = location(screenShaderProgram, "screenTexture");
    screenUniforms.bloomTexture = location(screenShaderProgram, "bloomTexture");
    screenUniforms.depthTexture = location(screenShaderProgram, "depthTexture");
    screenUniforms.blurEnabled = location(screenShaderProgram, "blurEnabled");
    screenUniforms.gammaCorrectionEnabled = location(screenShaderProgram, "gammaCorrectionEnabled");
    screenUniforms.hdrEnabled = location(screenShaderProgram, "hdrEnabled");
    screenUniforms.exposure = location(screenShaderProgram, "exposure");
    screenUniforms.bloomEnabled = location(screenShaderProgram, "bloomEnabled");
    screenUniforms.bloomStrength = location(screenShaderProgram, "bloomStrength");
    screenUniforms.depthDebugEnabled = location(screenShaderProgram, "depthDebugEnabled");
    screenUniforms.nearPlane = location(screenShaderProgram, "nearPlane");
    screenUniforms.farPlane = location(screenShaderProgram, "farPlane");

    brightPassUniforms.sceneTexture = location(brightPassShaderProgram, "sceneTexture");
    brightPassUniforms.brightnessThreshold = location(brightPassShaderProgram, "brightnessThreshold");
    gaussianBlurUniforms.image = location(gaussianBlurShaderProgram, "image");
    gaussianBlurUniforms.horizontal = location(gaussianBlurShaderProgram, "horizontal");

    directionalDepthUniforms.lightSpaceMatrix = location(directionalDepthShaderProgram, "lightSpaceMatrix");
    directionalDepthUniforms.model = location(directionalDepthShaderProgram, "model");
    instancedDirectionalDepthUniforms.lightSpaceMatrix =
        location(instancedDirectionalDepthShaderProgram, "lightSpaceMatrix");
    instancedDirectionalDepthUniforms.systemModel = location(instancedDirectionalDepthShaderProgram, "systemModel");
    pointDepthUniforms.lightPosition = location(pointDepthShaderProgram, "lightPos");
    pointDepthUniforms.farPlane = location(pointDepthShaderProgram, "farPlane");
    pointDepthUniforms.model = location(pointDepthShaderProgram, "model");
    for (std::size_t face = 0; face < pointDepthUniforms.shadowMatrices.size(); ++face)
    {
        pointDepthUniforms.shadowMatrices[face] =
            location(pointDepthShaderProgram, ("shadowMatrices[" + std::to_string(face) + "]").c_str());
    }

    cacheRoomPointLightUniforms(shaderProgram, lightingUniforms);
    cacheRoomPointLightUniforms(modelShaderProgram, modelUniforms);
    cacheRoomPointLightUniforms(normalMappingShaderProgram, normalMappingUniforms);

    cachePointShadowMapUniforms(shaderProgram, lightingUniforms);
    cachePointShadowMapUniforms(modelShaderProgram, modelUniforms);
    cachePointShadowMapUniforms(normalMappingShaderProgram, normalMappingUniforms);

    glUseProgram(shaderProgram);
    glUniform1i(lightingUniforms.directionalShadowMap, 3);
    bindPointShadowMaps(lightingUniforms, pointDepthCubemaps);

    glUseProgram(modelShaderProgram);
    glUniform1f(modelUniforms.materialShininess, 32.0f);
    bindPointShadowMaps(modelUniforms, pointDepthCubemaps);
    glUniform1i(modelUniforms.skybox, 2);

    glUseProgram(normalMappingShaderProgram);
    glUniform1i(normalMappingUniforms.normalDebugEnabled, normalDebugEnabled);
    bindPointShadowMaps(normalMappingUniforms, pointDepthCubemaps);
    glUniform1f(normalMappingUniforms.pointShadowFarPlane, POINT_SHADOW_FAR_PLANE);
    glUniform1i(normalMappingUniforms.pointShadowsEnabled, false);
    glUniform1i(normalMappingUniforms.diffuseMap, 0);
    glUniform1i(normalMappingUniforms.normalMap, 1);
    glUniform1i(normalMappingUniforms.depthMap, 2);

    glUseProgram(glassShaderProgram);
    glUniform1i(glassUniforms.texture, 0);

    glUseProgram(screenShaderProgram);
    glUniform1i(screenUniforms.screenTexture, 0);
    glUniform1i(screenUniforms.bloomTexture, 1);
    glUniform1i(screenUniforms.depthTexture, 2);

    glUseProgram(brightPassShaderProgram);
    glUniform1i(brightPassUniforms.sceneTexture, 0);

    glUseProgram(gaussianBlurShaderProgram);
    glUniform1i(gaussianBlurUniforms.image, 0);
}

void GraphicsScene::handleInput(GLFWwindow* window)
{
    toggleOnKeyPress(window, GLFW_KEY_1, key1WasPressed, movableLightModeEnabled);
    toggleOnKeyPress(window, GLFW_KEY_2, key2WasPressed, light2Enabled);

    if (toggleOnKeyPress(window, GLFW_KEY_N, keyNWasPressed, normalDebugEnabled) && normalDebugEnabled)
    {
        depthDebugEnabled = false;
    }

    toggleOnKeyPress(window, GLFW_KEY_F, keyFWasPressed, flashlightEnabled);

    if (toggleOnKeyPress(window, GLFW_KEY_V, keyVWasPressed, depthDebugEnabled) && depthDebugEnabled)
    {
        normalDebugEnabled = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        parallaxHeightScale = glm::max(0.0f, parallaxHeightScale - 0.0005f);
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        parallaxHeightScale = glm::min(0.1f, parallaxHeightScale + 0.0005f);
    }

    toggleOnKeyPress(window, GLFW_KEY_P, keyPWasPressed, outlineEnabled);
    toggleOnKeyPress(window, GLFW_KEY_G, keyGWasPressed, glassEnabled);
    toggleOnKeyPress(window, GLFW_KEY_B, keyBWasPressed, blurEnabled);
    toggleOnKeyPress(window, GLFW_KEY_M, keyMWasPressed, outdoorModeEnabled);
    toggleOnKeyPress(window, GLFW_KEY_H, keyHWasPressed, glassBackpackEnabled);
}

void GraphicsScene::draw(const glm::mat4& view,
                         const glm::mat4& projection,
                         const glm::vec3& cameraPos,
                         const glm::vec3& movableLightPos,
                         const glm::vec3& cameraFront)
{
    const RenderContext context{view, projection, cameraPos, cameraFront};
    const LightingState lighting{movableLightPos,
                                 glm::vec3(0.70f, 0.63f, 0.50f),
                                 glm::vec3(-2.0f, 2.2f, -2.0f),
                                 glm::vec3(0.3f, 0.5f, 1.0f),
                                 !outdoorModeEnabled && movableLightModeEnabled,
                                 !outdoorModeEnabled && light2Enabled,
                                 outdoorModeEnabled,
                                 flashlightEnabled};
    glm::mat4 lightSpaceMatrix(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if (outdoorModeEnabled)
    {
        lightSpaceMatrix = createDirectionalLightSpaceMatrix(sunDirection());

        renderDirectionalShadowMap(lightSpaceMatrix);
    }
    else if (movableLightModeEnabled)
    {
        renderPointShadowMap(lighting.primaryPosition, pointDepthCubemaps[0]);

        fixedPointShadowMapsValid[0] = false;
    }
    else
    {
        for (std::size_t i = 0; i < ROOM_POINT_LIGHT_COUNT; ++i)
        {
            if (fixedPointShadowMapsValid[i])
            {
                continue;
            }

            renderPointShadowMap(ROOM_POINT_LIGHT_POSITIONS[i], pointDepthCubemaps[i]);

            fixedPointShadowMapsValid[i] = true;
        }
    }

    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, multisampleFramebuffer);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (normalDebugEnabled)
    {
        glUseProgram(normalDebugShaderProgram);
        setMatrix(normalDebugUniforms.view, view);
        setMatrix(normalDebugUniforms.projection, projection);
        drawNormalDebugObjects();
        drawBackpackNormalDebug(view, projection);
    }

    glUseProgram(shaderProgram);
    setMatrix(lightingUniforms.lightSpaceMatrix, lightSpaceMatrix);
    bindTexture(GL_TEXTURE3, GL_TEXTURE_2D, directionalDepthMap);
    bindPointShadowMaps(lightingUniforms, pointDepthCubemaps);
    glUniform1f(lightingUniforms.pointShadowFarPlane, POINT_SHADOW_FAR_PLANE);
    glUniform1i(lightingUniforms.pointShadowsEnabled, !outdoorModeEnabled);
    configureLighting(lightingUniforms, context, lighting);
    uploadRoomPointLights(lightingUniforms, lighting.primaryPosition, movableLightModeEnabled, outdoorModeEnabled);

    glStencilMask(0x00);
    drawLitObjects();

    if (!outdoorModeEnabled)
    {
        glUseProgram(normalMappingShaderProgram);

        setVector(normalMappingUniforms.flashlightPos, context.cameraPosition);
        setVector(normalMappingUniforms.flashlightDir, context.cameraFront);
        glUniform1i(normalMappingUniforms.flashlightEnabled, lighting.flashlightEnabled);
        glUniform1f(normalMappingUniforms.flashlightCutOff, glm::cos(glm::radians(FLASHLIGHT_INNER_ANGLE)));
        glUniform1f(normalMappingUniforms.flashlightOuterCutOff, glm::cos(glm::radians(FLASHLIGHT_OUTER_ANGLE)));

        drawNormalMappedRoomWalls(context, lighting);
        drawNormalMappedFloorAndCeiling(context, lighting);
    }

    drawMiniPlanetSystem(context, lighting);

    if (outlineEnabled)
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        drawBackpack(context, lighting);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        drawBackpackOutline(view, projection);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
    }
    else
    {
        glStencilMask(0x00);
        drawBackpack(context, lighting);
        glStencilMask(0xFF);
    }

    glStencilMask(0x00);
    glUseProgram(lampShaderProgram);
    setMatrix(lampUniforms.view, view);
    setMatrix(lampUniforms.projection, projection);

    if (!outdoorModeEnabled)
    {
        if (movableLightModeEnabled)
        {
            drawLampMarker(lighting.primaryPosition, glm::vec3(1.0f, 1.0f, 0.3f));
        }
        else
        {
            for (const glm::vec3& fixedLightPosition : ROOM_POINT_LIGHT_POSITIONS)
            {
                drawLampMarker(fixedLightPosition, glm::vec3(1.0f, 1.0f, 0.3f));
            }
        }
        if (light2Enabled)
        {
            drawLampMarker(lighting.secondaryPosition, lighting.secondaryColor);
        }
    }

    glStencilMask(0xFF);

    if (skybox)
    {
        skybox->Render(projection);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glBlitFramebuffer(0,
                      0,
                      framebufferWidth,
                      framebufferHeight,
                      0,
                      0,
                      framebufferWidth,
                      framebufferHeight,
                      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
    extractBrightAreas();
    blurBrightAreas();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    drawScreenQuad();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
}

void GraphicsScene::drawLitObjects()
{
    setMaterial(shaderProgram, texturedMaterial);
    drawTexturedCubeWithSpecularMap(cube.VAO,
                                    lightingUniforms.model,
                                    makeTransform(glm::vec3(-0.2f, -0.6f, 0.0f), glm::vec3(1.0f)),
                                    containerTexture,
                                    containerSpecularTexture);

    setMaterial(shaderProgram, shinyBlue);
    drawSphere(sphere, lightingUniforms.model, makeTransform(glm::vec3(0.8f, -0.4f, 0.0f), glm::vec3(1.0f)));

    if (outdoorModeEnabled)
    {
        setMaterial(shaderProgram, outdoorGroundMaterial);
        drawCube(cube.VAO, lightingUniforms.model, createOutdoorGroundModelMatrix());
    }
}

void GraphicsScene::drawNormalDebugObjects()
{
    drawDebugPrimitives(cube, sphere, normalDebugUniforms.model, containerTexture);
}

void GraphicsScene::drawDepthDebugObjects()
{
    drawDebugPrimitives(cube, sphere, depthDebugUniforms.model, containerTexture);
}

void GraphicsScene::drawLampMarker(const glm::vec3& position, const glm::vec3& color, float scale)
{
    setVector(lampUniforms.color, color);

    drawCube(cube.VAO, lampUniforms.model, makeTransform(position, glm::vec3(scale)));
}

void GraphicsScene::drawBackpack(const RenderContext& context, const LightingState& lighting)
{
    if (!backpack)
    {
        return;
    }

    glUseProgram(modelShaderProgram);
    uploadRoomPointLights(modelUniforms, lighting.primaryPosition, movableLightModeEnabled, outdoorModeEnabled);
    configureModelPointShadows();
    glUniform1i(modelUniforms.glassBackpackEnabled, glassBackpackEnabled);
    if (skybox)
    {
        skybox->BindTexture(GL_TEXTURE2);
    }

    LightingState modelLighting = lighting;
    modelLighting.primaryColor = glm::vec3(1.0f);
    configureLighting(modelUniforms, context, modelLighting);

    setMatrix(modelUniforms.model, createBackpackModelMatrix());
    backpack->draw(modelShaderProgram);
}

void GraphicsScene::drawBackpackNormalDebug(const glm::mat4& view, const glm::mat4& projection)
{
    drawModelWithMatrices(backpack,
                          modelNormalDebugShaderProgram,
                          modelNormalDebugUniforms,
                          view,
                          projection,
                          createBackpackModelMatrix());
}

void GraphicsScene::drawBackpackOutline(const glm::mat4& view, const glm::mat4& projection)
{
    drawModelWithMatrices(backpack,
                          modelOutlineShaderProgram,
                          modelOutlineUniforms,
                          view,
                          projection,
                          createBackpackOutlineModelMatrix());
}

void GraphicsScene::drawBackpackDepthDebug(const glm::mat4& view, const glm::mat4& projection)
{
    if (!backpack)
    {
        return;
    }

    glUseProgram(modelDepthDebugShaderProgram);
    setMatrix(modelDepthDebugUniforms.view, view);
    setMatrix(modelDepthDebugUniforms.projection, projection);
    glUniform1f(modelDepthDebugUniforms.nearPlane, 0.1f);
    glUniform1f(modelDepthDebugUniforms.farPlane, 100.0f);
    setMatrix(modelDepthDebugUniforms.model, createBackpackModelMatrix());
    backpack->draw(modelDepthDebugShaderProgram);
}

void GraphicsScene::drawGlassPanes(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos)
{
    std::multimap<float, const GlassSurface*> sorted;

    for (const GlassSurface& surface : glassSurfaces)
    {
        if (surface.optional && !glassEnabled)
        {
            continue;
        }

        const float distance = glm::length(cameraPos - surface.position);
        sorted.emplace(distance, &surface);
    }

    glUseProgram(glassShaderProgram);
    setMatrix(glassUniforms.view, view);
    setMatrix(glassUniforms.projection, projection);
    bindTexture(GL_TEXTURE0, GL_TEXTURE_2D, glassTexture);

    const GLboolean cullingWasEnabled = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    glBindVertexArray(glassVAO);
    glDepthMask(GL_FALSE);
    glStencilMask(0x00);

    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        const GlassSurface& surface = *it->second;

        setMatrix(glassUniforms.model, makeTransform(surface.position, surface.scale, surface.yRotation));

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glStencilMask(0xFF);

    if (cullingWasEnabled)
    {
        glEnable(GL_CULL_FACE);
    }

    glActiveTexture(GL_TEXTURE0);
}

void GraphicsScene::drawMiniPlanetSystem(const RenderContext& context, const LightingState& lighting)
{
    if (!planet || !rock)
    {
        return;
    }

    const glm::vec3 mainLightColor(1.25f);
    const glm::mat4 planetModel = makeTransform(DISTANT_PLANET_POSITION, glm::vec3(DISTANT_PLANET_SCALE));
    const glm::mat4 asteroidSystemModel = createAsteroidSystemModel();

    glUseProgram(modelShaderProgram);
    uploadRoomPointLights(modelUniforms, lighting.primaryPosition, movableLightModeEnabled, true);
    configureModelPointShadows();
    LightingState planetLighting = lighting;
    planetLighting.primaryColor = mainLightColor;
    planetLighting.primaryEnabled = false;
    planetLighting.secondaryEnabled = false;
    planetLighting.directionalEnabled = true;
    planetLighting.flashlightEnabled = false;
    configureLighting(modelUniforms, context, planetLighting);
    glUniform1i(modelUniforms.pointShadowsEnabled, false);
    glUniform1i(modelUniforms.glassBackpackEnabled, false);
    glUniform1f(modelUniforms.materialShininess, 16.0f);
    setMatrix(modelUniforms.model, planetModel);
    planet->draw(modelShaderProgram);

    glUseProgram(instancedModelShaderProgram);
    const glm::mat4 lightSpaceMatrix = createDirectionalLightSpaceMatrix(sunDirection());
    setMatrix(instancedModelUniforms.lightSpaceMatrix, lightSpaceMatrix);
    bindTexture(GL_TEXTURE3, GL_TEXTURE_2D, directionalDepthMap);
    glUniform1i(instancedModelUniforms.directionalShadowMap, 3);
    setMatrix(instancedModelUniforms.systemModel, asteroidSystemModel);
    configureLighting(instancedModelUniforms, context, planetLighting);

    setVector(instancedModelUniforms.objectColor, glm::vec3(0.55f, 0.45f, 0.34f));
    setVector(instancedModelUniforms.objectSpecularColor, glm::vec3(0.08f));
    glUniform1f(instancedModelUniforms.shininess, 8.0f);
    rock->drawInstanced(instancedModelShaderProgram, ASTEROID_COUNT);
}

void GraphicsScene::cleanup()
{
    deleteCubeMesh(cube);
    deleteSphereMesh(sphere);

    const GLuint textures[] = {containerTexture,
                               containerSpecularTexture,
                               glassTexture,
                               framebufferTexture,
                               multisampleColorTexture,
                               directionalDepthMap,
                               brickDiffuseTexture,
                               brickNormalTexture,
                               brickDepthTexture,
                               floorDiffuseTexture,
                               floorNormalTexture,
                               ceilingDiffuseTexture,
                               ceilingNormalTexture,
                               pingPongTextures[0],
                               pingPongTextures[1],
                               framebufferDepthTexture,
                               brightTexture};
    glDeleteTextures(static_cast<GLsizei>(sizeof(textures) / sizeof(textures[0])), textures);
    glDeleteTextures(static_cast<GLsizei>(pointDepthCubemaps.size()), pointDepthCubemaps.data());

    const GLuint programs[] = {shaderProgram,
                               lampShaderProgram,
                               normalDebugShaderProgram,
                               modelShaderProgram,
                               modelNormalDebugShaderProgram,
                               depthDebugShaderProgram,
                               modelDepthDebugShaderProgram,
                               modelOutlineShaderProgram,
                               glassShaderProgram,
                               screenShaderProgram,
                               instancedModelShaderProgram,
                               directionalDepthShaderProgram,
                               instancedDirectionalDepthShaderProgram,
                               pointDepthShaderProgram,
                               normalMappingShaderProgram,
                               gaussianBlurShaderProgram,
                               brightPassShaderProgram};
    for (GLuint program : programs)
    {
        glDeleteProgram(program);
    }

    const GLuint framebuffers[] = {framebuffer,
                                   multisampleFramebuffer,
                                   directionalDepthFBO,
                                   pointDepthFBO,
                                   pingPongFBO[0],
                                   pingPongFBO[1],
                                   brightPassFBO};
    glDeleteFramebuffers(static_cast<GLsizei>(sizeof(framebuffers) / sizeof(framebuffers[0])), framebuffers);

    const GLuint vertexArrays[] = {glassVAO, screenQuadVAO, normalMappedPlaneVAO};
    glDeleteVertexArrays(static_cast<GLsizei>(sizeof(vertexArrays) / sizeof(vertexArrays[0])), vertexArrays);

    const GLuint buffers[] = {glassVBO, screenQuadVBO, asteroidInstanceVBO, normalMappedPlaneVBO};
    glDeleteBuffers(static_cast<GLsizei>(sizeof(buffers) / sizeof(buffers[0])), buffers);
    glDeleteRenderbuffers(1, &multisampleRBO);

    delete backpack;
    backpack = nullptr;
    delete skybox;
    skybox = nullptr;
    delete planet;
    planet = nullptr;
    delete rock;
    rock = nullptr;

    clearUniformLocationCache();
}
