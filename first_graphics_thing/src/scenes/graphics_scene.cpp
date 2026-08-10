#include "week09_scene.h"

#include "../common/shader.h"
#include "../common/texture.h"

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

namespace
{
    constexpr float FLASHLIGHT_INNER_ANGLE = 10.0f;
    constexpr float FLASHLIGHT_OUTER_ANGLE = 28.0f;

    const glm::vec3 DISTANT_PLANET_POSITION(
        0.0f,
        6.0f,
        -80.0f
    );

    constexpr float DISTANT_PLANET_SCALE = 12.0f;

    constexpr float DISTANT_ASTEROID_SYSTEM_SCALE = 1.20f;

    GLint uniformLocation(GLuint program, const char* name)
    {
        return glGetUniformLocation(program, name);
    }

    void setMatrix(GLint location, const glm::mat4& value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setVector(GLint location, const glm::vec3& value)
    {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void bindTexture(GLenum unit, GLenum target, GLuint texture)
    {
        glActiveTexture(unit);
        glBindTexture(target, texture);
    }

    glm::mat4 makeTransform(
        const glm::vec3& position,
        const glm::vec3& scale,
        float angleDegrees = 0.0f,
        const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f))
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        if (angleDegrees != 0.0f)
        {
            model = glm::rotate(model, glm::radians(angleDegrees), axis);
        }
        return glm::scale(model, scale);
    }

    glm::vec3 sunDirection()
    {
        return glm::vec3(-0.3f, -1.0f, -0.2f);
    }

    glm::vec3 sunColor()
    {
        return glm::vec3(0.45f, 0.43f, 0.36f);
    }

    const glm::vec3 WALL_LAMP_POSITION(
        2.86f,
        1.15f,
        0.0f
    );

    const glm::vec3 WALL_LIGHT_POSITION(
        2.60f,
        1.30f,
        0.0f
    );

    const glm::vec3 WALL_LIGHT_COLOR(
        0.70f,
        0.63f,
        0.50f
    );

    bool toggleOnKeyPress(GLFWwindow* window, int key, bool& wasPressed, bool& value)
    {
        const bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
        const bool toggled = isPressed && !wasPressed;
        if (toggled)
        {
            value = !value;
        }
        wasPressed = isPressed;
        return toggled;
    }

    void configureVertexAttribute(
        GLuint index,
        GLint componentCount,
        GLsizei stride,
        std::size_t byteOffset)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(
            index,
            componentCount,
            GL_FLOAT,
            GL_FALSE,
            stride,
            reinterpret_cast<void*>(byteOffset));
    }

    void drawDebugPrimitives(
        const CubeMesh& cube,
        const SphereMesh& sphere,
        GLint modelLocation,
        GLuint texture)
    {
        drawTexturedCube(
            cube.VAO,
            modelLocation,
            makeTransform(glm::vec3(-0.2f, -0.6f, 0.0f), glm::vec3(1.0f)),
            texture);

        drawSphere(
            sphere,
            modelLocation,
            makeTransform(glm::vec3(0.8f, -0.4f, 0.0f), glm::vec3(1.0f)));

        drawClosedRoom(cube.VAO, modelLocation, texture);
    }

    template <typename Uniforms>
    void configureModelLighting(
        const Uniforms& uniforms,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& cameraFront,
        const glm::vec3& lightPos,
        const glm::vec3& lightColor,
        const glm::vec3& secondLightPos,
        const glm::vec3& secondLightColor,
        bool light1Enabled,
        bool light2Enabled,
        bool directionalLightEnabled,
        bool flashlightEnabled)
    {
        setMatrix(uniforms.view, view);
        setMatrix(uniforms.projection, projection);
        setVector(uniforms.viewPos, cameraPos);
        setVector(uniforms.lightPos, lightPos);
        setVector(uniforms.lightColor, lightColor);
        setVector(uniforms.secondLightPos, secondLightPos);
        setVector(uniforms.secondLightColor, secondLightColor);
        glUniform1i(uniforms.light1Enabled, light1Enabled);
        glUniform1i(uniforms.light2Enabled, light2Enabled);
        glUniform1i(uniforms.directionalLightEnabled, directionalLightEnabled);
        setVector(uniforms.directionalLightDirection, sunDirection());
        setVector(uniforms.directionalLightColor, sunColor());
        setVector(uniforms.flashlightPos, cameraPos);
        setVector(uniforms.flashlightDir, cameraFront);
        glUniform1i(uniforms.flashlightEnabled, flashlightEnabled);
        glUniform1f(uniforms.flashlightCutOff, glm::cos(glm::radians(FLASHLIGHT_INNER_ANGLE)));
        glUniform1f(uniforms.flashlightOuterCutOff, glm::cos(glm::radians(FLASHLIGHT_OUTER_ANGLE)));
    }

    void configureInstancedLighting(
        GLuint program,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& cameraFront,
        const glm::vec3& lightPos,
        const glm::vec3& lightColor,
        const glm::vec3& secondLightPos,
        const glm::vec3& secondLightColor,
        bool light1Enabled,
        bool light2Enabled,
        bool directionalLightEnabled,
        bool flashlightEnabled)
    {
        setMatrix(uniformLocation(program, "view"), view);
        setMatrix(uniformLocation(program, "projection"), projection);
        setVector(uniformLocation(program, "viewPos"), cameraPos);
        setVector(uniformLocation(program, "lightPos"), lightPos);
        setVector(uniformLocation(program, "lightColor"), lightColor);
        setVector(uniformLocation(program, "secondLightPos"), secondLightPos);
        setVector(uniformLocation(program, "secondLightColor"), secondLightColor);
        glUniform1i(uniformLocation(program, "light1Enabled"), light1Enabled);
        glUniform1i(uniformLocation(program, "light2Enabled"), light2Enabled);
        glUniform1i(uniformLocation(program, "directionalLightEnabled"), directionalLightEnabled);
        setVector(uniformLocation(program, "directionalLightDirection"), sunDirection());
        setVector(uniformLocation(program, "directionalLightColor"), sunColor());
        setVector(uniformLocation(program, "flashlightPos"), cameraPos);
        setVector(uniformLocation(program, "flashlightDir"), cameraFront);
        glUniform1i(uniformLocation(program, "flashlightEnabled"), flashlightEnabled);
        glUniform1f(
            uniformLocation(program, "flashlightCutOff"),
            glm::cos(glm::radians(FLASHLIGHT_INNER_ANGLE)));
        glUniform1f(
            uniformLocation(program, "flashlightOuterCutOff"),
            glm::cos(glm::radians(FLASHLIGHT_OUTER_ANGLE)));
    }

    template <typename Uniforms>
    void drawModelWithMatrices(
        Model* model,
        GLuint program,
        const Uniforms& uniforms,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::mat4& modelMatrix)
    {
        if (!model)
        {
            return;
        }

        glUseProgram(program);
        setMatrix(uniforms.view, view);
        setMatrix(uniforms.projection, projection);
        setMatrix(uniforms.model, modelMatrix);
        model->draw(program);
    }

    struct TangentBasis
    {
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    TangentBasis calculateTangentBasis(
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3,
        const glm::vec2& uv1,
        const glm::vec2& uv2,
        const glm::vec2& uv3)
    {
        const glm::vec3 edge1 = p2 - p1;
        const glm::vec3 edge2 = p3 - p1;
        const glm::vec2 deltaUV1 = uv2 - uv1;
        const glm::vec2 deltaUV2 = uv3 - uv1;
        const float inverseDeterminant = 1.0f /
            (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        TangentBasis basis;
        basis.tangent = glm::normalize(
            inverseDeterminant * (deltaUV2.y * edge1 - deltaUV1.y * edge2));
        basis.bitangent = glm::normalize(
            inverseDeterminant * (-deltaUV2.x * edge1 + deltaUV1.x * edge2));
        return basis;
    }
}

void Week09Scene::init(int width, int height, FreeCamera* camera)
{
    shaderProgram = createShaderProgram("src/week09/vertex.glsl", "src/week09/fragment.glsl");
    lampShaderProgram = createShaderProgram("src/week09/lamp_vertex.glsl", "src/week09/lamp_fragment.glsl");
    normalDebugShaderProgram = createShaderProgram("src/week09/vertex.glsl", "src/week09/normal_debug_fragment.glsl");
    modelShaderProgram = createShaderProgram("src/week09/model_vertex.glsl", "src/week09/model_fragment.glsl");
    modelNormalDebugShaderProgram = createShaderProgram("src/week09/model_vertex.glsl", "src/week09/normal_debug_fragment.glsl");
    depthDebugShaderProgram = createShaderProgram("src/week09/vertex.glsl", "src/week09/depth_debug_fragment.glsl");
    modelDepthDebugShaderProgram = createShaderProgram("src/week09/model_vertex.glsl", "src/week09/depth_debug_fragment.glsl");
    modelOutlineShaderProgram = createShaderProgram("src/week09/model_vertex.glsl", "src/week09/single_color_fragment.glsl");
    glassShaderProgram = createShaderProgram("src/week09/glass_vertex.glsl", "src/week09/glass_fragment.glsl");
    screenShaderProgram = createShaderProgram("src/week09/screen_vertex.glsl", "src/week09/screen_fragment.glsl");
    brightPassShaderProgram = createShaderProgram("src/week09/screen_vertex.glsl", "src/week09/bright_pass_fragment.glsl");
    gaussianBlurShaderProgram = createShaderProgram("src/week09/screen_vertex.glsl","src/week09/gaussian_blur_fragment.glsl");
    instancedModelShaderProgram = createShaderProgram("src/week09/instanced_model_vertex.glsl", "src/week09/instanced_rock_fragment.glsl");
    directionalDepthShaderProgram = createShaderProgram("src/week09/directional_depth_vertex.glsl", "src/week09/directional_depth_fragment.glsl");
    instancedDirectionalDepthShaderProgram = createShaderProgram("src/week09/instanced_directional_depth_vertex.glsl", "src/week09/directional_depth_fragment.glsl");
    pointDepthShaderProgram = createShaderProgram("src/week09/point_depth_vertex.glsl", "src/week09/point_depth_geometry.glsl", "src/week09/point_depth_fragment.glsl");
    normalMappingShaderProgram = createShaderProgram("src/week09/normal_mapping_vertex.glsl", "src/week09/normal_mapping_fragment.glsl");

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
    wallLamp = new Model("assets/models/industrial_wall_lamp/industrial_wall_lamp_1k.gltf");
    cube = createCubeMesh();
    sphere = createSphereMesh();

    createAsteroidInstances();
    createGlassPaneMesh();
    createFramebuffer(width, height);
    createBloomFramebuffer(width, height);
    createPingPongFramebuffers(width,height);
    createScreenQuad();
    createDirectionalShadowMap();
    createPointShadowMap();
    createNormalMappedPlane();

    glassSurfaces = {
        { glm::vec3(0.0f, 1.0f, 2.94f), glm::vec3(6.0f, 4.0f, 1.0f), 180.0f, false },   //glass wall
        { glm::vec3(-1.8f, 0.0f, -1.8f), glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true },
        { glm::vec3(0.0f, 0.0f, -1.8f),  glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true },
        { glm::vec3(1.8f, 0.0f, -1.8f),  glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true },
        { glm::vec3(-1.2f, 0.0f, 0.8f),  glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true },
        { glm::vec3(1.2f, 0.0f, 0.8f),   glm::vec3(1.0f, 1.4f, 1.0f), 0.0f, true }
    };

    outdoorGroundMaterial.diffuseColor = glm::vec3(0.18f, 0.42f, 0.16f);
    outdoorGroundMaterial.specularColor = glm::vec3(0.05f);
    outdoorGroundMaterial.shininess = 8.0f;
    outdoorGroundMaterial.useDiffuseMap = false;
    outdoorGroundMaterial.useSpecularMap = false;
    dullOrange = createDullOrangeMaterial();
    shinyBlue = createShinyBlueMaterial();
    matteGray = createMatteGrayMaterial();
    texturedMaterial = createTexturedMaterial();

    skybox = new SkyBox(camera);
    if (!skybox->Init(
        "assets/textures/space",
        "px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png"))
    {
        std::cout << "Skybox failed to load\n";
    }

    const auto location = [](GLuint program, const char* name) {
        return static_cast<GLuint>(glGetUniformLocation(program, name));
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
    lightingUniforms.pointShadowMap = location(shaderProgram, "pointShadowMap");
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
    modelUniforms.pointShadowMap = location(modelShaderProgram, "pointShadowMap");
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
    modelUniforms.wallLightPos = location(modelShaderProgram, "wallLightPos");
    modelUniforms.wallLightColor = location(modelShaderProgram, "wallLightColor");
    modelUniforms.wallLightEnabled = location(modelShaderProgram, "wallLightEnabled");

    modelOutlineUniforms.model = location(modelOutlineShaderProgram, "model");
    modelOutlineUniforms.view = location(modelOutlineShaderProgram, "view");
    modelOutlineUniforms.projection = location(modelOutlineShaderProgram, "projection");
    glassUniforms.model = location(glassShaderProgram, "model");
    glassUniforms.view = location(glassShaderProgram, "view");
    glassUniforms.projection = location(glassShaderProgram, "projection");

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
    normalMappingUniforms.pointShadowMap = location(normalMappingShaderProgram, "pointShadowMap");
    normalMappingUniforms.pointShadowFarPlane = location(normalMappingShaderProgram, "pointShadowFarPlane");
    normalMappingUniforms.pointShadowsEnabled = location(normalMappingShaderProgram, "pointShadowsEnabled");
    normalMappingUniforms.wallLightPos = location(normalMappingShaderProgram, "wallLightPos");
    normalMappingUniforms.wallLightColor = location(normalMappingShaderProgram, "wallLightColor");
    normalMappingUniforms.wallLightEnabled = location(normalMappingShaderProgram, "wallLightEnabled");

    glUseProgram(shaderProgram);
    glUniform1i(lightingUniforms.directionalShadowMap, 3);
    glUniform1i(lightingUniforms.pointShadowMap, 4);

    glUseProgram(modelShaderProgram);
    glUniform1f(uniformLocation(modelShaderProgram, "material.shininess"), 32.0f);
    glUniform1i(modelUniforms.pointShadowMap, 4);
    glUniform1i(modelUniforms.skybox, 2);

    glUseProgram(normalMappingShaderProgram);
    bindTexture(GL_TEXTURE4, GL_TEXTURE_CUBE_MAP, pointDepthCubemap);
    glUniform1f(normalMappingUniforms.pointShadowFarPlane, POINT_SHADOW_FAR_PLANE);
    glUniform1i(normalMappingUniforms.pointShadowsEnabled, !outdoorModeEnabled && !light1Enabled);
    glUniform1i(normalMappingUniforms.diffuseMap, 0);
    glUniform1i(normalMappingUniforms.normalMap, 1);
    glUniform1i(normalMappingUniforms.depthMap, 2);
    glUniform1i(normalMappingUniforms.pointShadowMap, 4);

    glUseProgram(glassShaderProgram);
    glUniform1i(uniformLocation(glassShaderProgram, "glassTexture"), 0);

    glUseProgram(screenShaderProgram);
    screenTextureUniform = location(screenShaderProgram, "screenTexture");
    glUniform1i(screenTextureUniform, 0);

    glUniform1i(uniformLocation(screenShaderProgram,"bloomTexture"),1);

    glUseProgram(brightPassShaderProgram);
    glUniform1i(uniformLocation(brightPassShaderProgram, "sceneTexture"), 0);

    glUseProgram(gaussianBlurShaderProgram);
    glUniform1i(uniformLocation(gaussianBlurShaderProgram,"image"),0);
}

void Week09Scene::handleInput(GLFWwindow* window)
{
    toggleOnKeyPress(window, GLFW_KEY_1, key1WasPressed, light1Enabled);
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

void Week09Scene::draw(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& movableLightPos,
    const glm::vec3& cameraFront)
{
    const glm::vec3 lightPos = movableLightPos;
    const glm::vec3 lightColor(0.70f, 0.63f, 0.50f);
    const glm::vec3 secondLightPos(-2.0f, 2.2f, -2.0f);
    const glm::vec3 secondLightColor(0.3f, 0.5f, 1.0f);
    const bool wallLightEnabled = !outdoorModeEnabled && !light1Enabled;
    glm::mat4 lightSpaceMatrix(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if (outdoorModeEnabled)
    {
        lightSpaceMatrix = createDirectionalLightSpaceMatrix(sunDirection());
        renderDirectionalShadowMap(lightSpaceMatrix);
    }
    else if (light1Enabled)
    {
        renderPointShadowMap(lightPos);
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
    else if (depthDebugEnabled)
    {
        glUseProgram(depthDebugShaderProgram);
        setMatrix(depthDebugUniforms.view, view);
        setMatrix(depthDebugUniforms.projection, projection);
        glUniform1f(depthDebugUniforms.nearPlane, 0.1f);
        glUniform1f(depthDebugUniforms.farPlane, 100.0f);
        drawDepthDebugObjects();
        drawBackpackDepthDebug(view, projection);
    }

    glUseProgram(shaderProgram);
    setMatrix(lightingUniforms.lightSpaceMatrix, lightSpaceMatrix);
    bindTexture(GL_TEXTURE3, GL_TEXTURE_2D, directionalDepthMap);
    bindTexture(GL_TEXTURE4, GL_TEXTURE_CUBE_MAP, pointDepthCubemap);
    glUniform1f(lightingUniforms.pointShadowFarPlane, POINT_SHADOW_FAR_PLANE);
    glUniform1i(lightingUniforms.pointShadowsEnabled, !outdoorModeEnabled && light1Enabled);
    glUniform1i(lightingUniforms.light1Enabled, !outdoorModeEnabled && light1Enabled);
    glUniform1i(lightingUniforms.light2Enabled, !outdoorModeEnabled && light2Enabled);
    glUniform1i(lightingUniforms.directionalLightEnabled, outdoorModeEnabled);
    setVector(lightingUniforms.directionalLightDirection, sunDirection());
    setVector(lightingUniforms.directionalLightColor, sunColor());
    setMatrix(lightingUniforms.view, view);
    setMatrix(lightingUniforms.projection, projection);
    setVector(lightingUniforms.viewPos, cameraPos);
    setVector(lightingUniforms.lightPos, lightPos);
    setVector(lightingUniforms.lightColor, lightColor);
    setVector(lightingUniforms.secondLightPos, secondLightPos);
    setVector(lightingUniforms.secondLightColor, secondLightColor);

    setVector(lightingUniforms.wallLightPos, WALL_LIGHT_POSITION);
    setVector(lightingUniforms.wallLightColor, WALL_LIGHT_COLOR);
    glUniform1i(lightingUniforms.wallLightEnabled, wallLightEnabled);

    setVector(lightingUniforms.flashlightPos, cameraPos);
    setVector(lightingUniforms.flashlightDir, cameraFront);
    glUniform1i(lightingUniforms.flashlightEnabled, flashlightEnabled);
    glUniform1f(lightingUniforms.flashlightCutOff, glm::cos(glm::radians(FLASHLIGHT_INNER_ANGLE)));
    glUniform1f(lightingUniforms.flashlightOuterCutOff, glm::cos(glm::radians(FLASHLIGHT_OUTER_ANGLE)));

    glUseProgram(modelShaderProgram);

    setVector(modelUniforms.wallLightPos, WALL_LIGHT_POSITION);
    setVector(modelUniforms.wallLightColor, WALL_LIGHT_COLOR);
    glUniform1i(modelUniforms.wallLightEnabled, wallLightEnabled);

    glUseProgram(shaderProgram);

    glStencilMask(0x00);
    drawLitObjects();

    if (!outdoorModeEnabled)
    {
        glUseProgram(normalMappingShaderProgram);

        setVector(normalMappingUniforms.wallLightPos, WALL_LIGHT_POSITION);
        setVector(normalMappingUniforms.wallLightColor, WALL_LIGHT_COLOR);
        glUniform1i(normalMappingUniforms.wallLightEnabled, wallLightEnabled);

        drawNormalMappedRoomWalls(view, projection, cameraPos, lightPos, lightColor);
        drawNormalMappedFloorAndCeiling(view, projection, cameraPos, lightPos, lightColor);
    }

    drawMiniPlanetSystem(
        view,
        projection,
        cameraPos,
        cameraFront,
        lightPos,
        secondLightPos,
        secondLightColor);

    if (outlineEnabled)
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        drawBackpack(view, projection, cameraPos, cameraFront, lightPos, secondLightPos, secondLightColor);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        drawBackpackOutline(view, projection);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
    }
    else
    {
        glStencilMask(0x00);
        drawBackpack(view, projection, cameraPos, cameraFront, lightPos, secondLightPos, secondLightColor);
        glStencilMask(0xFF);
    }

    if (wallLamp && !outdoorModeEnabled)
    {
        glUseProgram(modelShaderProgram);

        configureModelPointShadows();

        glUniform1i(
            modelUniforms.glassBackpackEnabled,
            false
        );

        configureModelLighting(
            modelUniforms,
            view,
            projection,
            cameraPos,
            cameraFront,
            lightPos,
            lightColor,
            secondLightPos,
            secondLightColor,
            !outdoorModeEnabled && light1Enabled,
            !outdoorModeEnabled && light2Enabled,
            outdoorModeEnabled,
            flashlightEnabled
        );

        setVector(
            modelUniforms.wallLightPos,
            WALL_LIGHT_POSITION
        );
        setVector(
            modelUniforms.wallLightColor,
            WALL_LIGHT_COLOR
        );
        glUniform1i(
            modelUniforms.wallLightEnabled,
            wallLightEnabled
        );

        setMatrix(
            modelUniforms.model,
            createWallLampModelMatrix()
        );

        wallLamp->draw(modelShaderProgram);
    }

    glStencilMask(0x00);
    glUseProgram(lampShaderProgram);

    glStencilMask(0x00);
    glUseProgram(lampShaderProgram);
    setMatrix(lampUniforms.view, view);
    setMatrix(lampUniforms.projection, projection);

    if (!outdoorModeEnabled)
    {
        if (light1Enabled)
        {
            drawLampMarker(
                lightPos,
                glm::vec3(1.0f, 1.0f, 0.3f)
            );
        }

        if (wallLightEnabled)
        {
            drawLampMarker(
                WALL_LIGHT_POSITION,
                glm::vec3(1.0f, 0.75f, 0.25f),
                0.045f
            );
        }
        if (light2Enabled)
        {
            drawLampMarker(secondLightPos, glm::vec3(0.3f, 0.5f, 1.0f));
        }
    }

    glStencilMask(0xFF);

    if (skybox)
    {
        skybox->Render(projection);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glBlitFramebuffer(
        0, 0, framebufferWidth, framebufferHeight,
        0, 0, framebufferWidth, framebufferHeight,
        GL_COLOR_BUFFER_BIT,
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

void Week09Scene::drawLitObjects()
{
    setMaterial(shaderProgram, texturedMaterial);
    drawTexturedCubeWithSpecularMap(
        cube.VAO,
        lightingUniforms.model,
        makeTransform(glm::vec3(-0.2f, -0.6f, 0.0f), glm::vec3(1.0f)),
        containerTexture,
        containerSpecularTexture);

    setMaterial(shaderProgram, shinyBlue);
    drawSphere(
        sphere,
        lightingUniforms.model,
        makeTransform(glm::vec3(0.8f, -0.4f, 0.0f), glm::vec3(1.0f)));

    if (outdoorModeEnabled)
    {
        setMaterial(shaderProgram, outdoorGroundMaterial);
        drawCube(cube.VAO, lightingUniforms.model, createOutdoorGroundModelMatrix());
    }
}

glm::mat4 Week09Scene::createOutdoorGroundModelMatrix() const
{
    return makeTransform(glm::vec3(0.0f, -0.95f, 0.0f), glm::vec3(20.0f, 0.1f, 20.0f));
}

void Week09Scene::drawNormalDebugObjects()
{
    drawDebugPrimitives(cube, sphere, normalDebugUniforms.model, containerTexture);
}

void Week09Scene::drawDepthDebugObjects()
{
    drawDebugPrimitives(cube, sphere, depthDebugUniforms.model, containerTexture);
}

void Week09Scene::drawLampMarker(
    const glm::vec3& position,
    const glm::vec3& color,
    float scale)
{
    setVector(lampUniforms.color, color);

    drawCube(
        cube.VAO,
        lampUniforms.model,
        makeTransform(position, glm::vec3(scale)));
}

void Week09Scene::drawBackpack(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& cameraFront,
    const glm::vec3& lightPos,
    const glm::vec3& secondLightPos,
    const glm::vec3& secondLightColor)
{
    if (!backpack)
    {
        return;
    }

    glUseProgram(modelShaderProgram);
    configureModelPointShadows();
    glUniform1i(modelUniforms.glassBackpackEnabled, glassBackpackEnabled);
    if (skybox)
    {
        skybox->BindTexture(GL_TEXTURE2);
    }

    configureModelLighting(
        modelUniforms,
        view,
        projection,
        cameraPos,
        cameraFront,
        lightPos,
        glm::vec3(1.0f),
        secondLightPos,
        secondLightColor,
        !outdoorModeEnabled && light1Enabled,
        !outdoorModeEnabled && light2Enabled,
        outdoorModeEnabled,
        flashlightEnabled);

    setMatrix(modelUniforms.model, createBackpackModelMatrix());
    backpack->draw(modelShaderProgram);
}

glm::mat4 Week09Scene::createBackpackOutlineModelMatrix() const
{
    return makeTransform(
        glm::vec3(-1.0f, -0.5f, 0.0f),
        glm::vec3(0.215f),
        90.0f,
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void Week09Scene::drawBackpackNormalDebug(const glm::mat4& view, const glm::mat4& projection)
{
    drawModelWithMatrices(
        backpack,
        modelNormalDebugShaderProgram,
        modelNormalDebugUniforms,
        view,
        projection,
        createBackpackModelMatrix());
}

void Week09Scene::drawBackpackOutline(const glm::mat4& view, const glm::mat4& projection)
{
    drawModelWithMatrices(
        backpack,
        modelOutlineShaderProgram,
        modelOutlineUniforms,
        view,
        projection,
        createBackpackOutlineModelMatrix());
}

void Week09Scene::drawBackpackDepthDebug(const glm::mat4& view, const glm::mat4& projection)
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

glm::mat4 Week09Scene::createBackpackModelMatrix() const
{
    return makeTransform(
        glm::vec3(-1.0f, -0.5f, 0.0f),
        glm::vec3(0.205f),
        90.0f,
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void Week09Scene::createGlassPaneMesh()
{
    const float vertices[] = {
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &glassVAO);
    glGenBuffers(1, &glassVBO);
    glBindVertexArray(glassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, glassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    configureVertexAttribute(0, 3, 5 * sizeof(float), 0);
    configureVertexAttribute(1, 2, 5 * sizeof(float), 3 * sizeof(float));
    glBindVertexArray(0);
}

void Week09Scene::drawGlassPanes(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos)
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

        setMatrix(
            glassUniforms.model,
            makeTransform(surface.position, surface.scale, surface.yRotation)
        );

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

void Week09Scene::createFramebuffer(int width, int height)
{
    framebufferWidth = width;
    framebufferHeight = height;

    glGenFramebuffers(1, &multisampleFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, multisampleFramebuffer);
    glGenTextures(1, &multisampleColorTexture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleColorTexture);
    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE,
        MSAA_SAMPLES,
        GL_RGB16F,
        width,
        height,
        GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE,
        multisampleColorTexture,
        0);

    glGenRenderbuffers(1, &multisampleRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, multisampleRBO);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER,
        MSAA_SAMPLES,
        GL_DEPTH24_STENCIL8,
        width,
        height);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        multisampleRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Multisample framebuffer is not complete!" << std::endl;
    }

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        framebufferTexture,
        0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Resolved framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Week09Scene::createBloomFramebuffer(
    int width,
    int height
)
{
    glGenFramebuffers(1, &brightPassFBO);
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        brightPassFBO
    );

    glGenTextures(1, &brightTexture);
    glBindTexture(
        GL_TEXTURE_2D,
        brightTexture
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        width,
        height,
        0,
        GL_RGB,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        brightTexture,
        0
    );

    if (
        glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE
        )
    {
        std::cout
            << "ERROR::FRAMEBUFFER:: Bright-pass "
            << "framebuffer is not complete!"
            << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Week09Scene::createPingPongFramebuffers(
    int width,
    int height
)
{
    glGenFramebuffers(
        2,
        pingPongFBO
    );

    glGenTextures(
        2,
        pingPongTextures
    );

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(
            GL_FRAMEBUFFER,
            pingPongFBO[i]
        );

        glBindTexture(
            GL_TEXTURE_2D,
            pingPongTextures[i]
        );

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB16F,
            width,
            height,
            0,
            GL_RGB,
            GL_FLOAT,
            nullptr
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE
        );

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            pingPongTextures[i],
            0
        );

        if (
            glCheckFramebufferStatus(
                GL_FRAMEBUFFER
            ) != GL_FRAMEBUFFER_COMPLETE
            )
        {
            std::cout
                << "ERROR::FRAMEBUFFER:: Ping-pong "
                << i
                << " framebuffer is not complete!"
                << std::endl;
        }
    }

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

void Week09Scene::extractBrightAreas()
{
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        brightPassFBO
    );

    glViewport(
        0,
        0,
        framebufferWidth,
        framebufferHeight
    );

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(brightPassShaderProgram);

    glUniform1f(
        uniformLocation(
            brightPassShaderProgram,
            "brightnessThreshold"
        ),
        1.0f
    );

    bindTexture(
        GL_TEXTURE0,
        GL_TEXTURE_2D,
        framebufferTexture
    );

    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Week09Scene::blurBrightAreas()
{
    constexpr unsigned int blurPassCount = 10;

    bool horizontal = true;
    bool firstPass = true;

    glUseProgram(
        gaussianBlurShaderProgram
    );

    glBindVertexArray(
        screenQuadVAO
    );

    for (
        unsigned int pass = 0;
        pass < blurPassCount;
        pass++
        )
    {
        glBindFramebuffer(
            GL_FRAMEBUFFER,
            pingPongFBO[horizontal]
        );

        glUniform1i(
            uniformLocation(
                gaussianBlurShaderProgram,
                "horizontal"
            ),
            horizontal
        );

        GLuint inputTexture;

        if (firstPass)
        {
            inputTexture = brightTexture;
        }
        else
        {
            inputTexture =
                pingPongTextures[!horizontal];
        }

        bindTexture(
            GL_TEXTURE0,
            GL_TEXTURE_2D,
            inputTexture
        );

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            6
        );

        horizontal = !horizontal;
        firstPass = false;
    }

    glBindVertexArray(0);

    blurredBloomTexture =
        pingPongTextures[!horizontal];

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

void Week09Scene::createScreenQuad()
{
    const float vertices[] = {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &screenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);
    glBindVertexArray(screenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    configureVertexAttribute(0, 2, 4 * sizeof(float), 0);
    configureVertexAttribute(1, 2, 4 * sizeof(float), 2 * sizeof(float));
    glBindVertexArray(0);
}

void Week09Scene::drawScreenQuad()
{
    glUseProgram(screenShaderProgram);

    glUniform1i(uniformLocation(screenShaderProgram, "blurEnabled"), blurEnabled);
    glUniform1i(uniformLocation(screenShaderProgram, "gammaCorrectionEnabled"), true);
    glUniform1i(uniformLocation(screenShaderProgram, "hdrEnabled"), true);
    glUniform1f(uniformLocation(screenShaderProgram, "exposure"), 1.2f);
    glUniform1i(uniformLocation(screenShaderProgram, "bloomEnabled"), bloomEnabled);
    glUniform1f(uniformLocation(screenShaderProgram, "bloomStrength"), bloomStrength);

    bindTexture(GL_TEXTURE0, GL_TEXTURE_2D, framebufferTexture);
    bindTexture(GL_TEXTURE1, GL_TEXTURE_2D, blurredBloomTexture);

    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Week09Scene::createAsteroidInstances()
{
    if (!rock)
    {
        return;
    }

    asteroidMatrices.resize(ASTEROID_COUNT);
    std::mt19937 randomEngine(1337);
    std::uniform_real_distribution<float> displacement(-8.0f, 8.0f);
    std::uniform_real_distribution<float> scaleDistribution(0.05f, 0.12f);
    std::uniform_real_distribution<float> rotationDistribution(0.0f, glm::two_pi<float>());
    const glm::vec3 rotationAxis = glm::normalize(glm::vec3(0.4f, 0.6f, 0.8f));
    constexpr float radius = 50.0f;

    for (unsigned int i = 0; i < ASTEROID_COUNT; ++i)
    {
        const float percentage = static_cast<float>(i) / static_cast<float>(ASTEROID_COUNT);
        const float angle = percentage * glm::two_pi<float>();
        const float x = std::sin(angle) * radius + displacement(randomEngine);
        const float y = displacement(randomEngine) * 0.25f;
        const float z = std::cos(angle) * radius + displacement(randomEngine);
        const glm::vec3 position(x, y, z);
        const float scale = scaleDistribution(randomEngine);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, glm::vec3(scale));
        asteroidMatrices[i] = glm::rotate(model, rotationDistribution(randomEngine), rotationAxis);
    }

    glGenBuffers(1, &asteroidInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, asteroidInstanceVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        asteroidMatrices.size() * sizeof(glm::mat4),
        asteroidMatrices.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    rock->configureInstancing(asteroidInstanceVBO);
}

void Week09Scene::drawMiniPlanetSystem(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& cameraFront,
    const glm::vec3& lightPos,
    const glm::vec3& secondLightPos,
    const glm::vec3& secondLightColor)
{
    if (!planet || !rock)
    {
        return;
    }

    const glm::vec3 mainLightColor(1.25f);
    const glm::mat4 planetModel = makeTransform(
        DISTANT_PLANET_POSITION,
        glm::vec3(DISTANT_PLANET_SCALE)
    );
    const glm::mat4 asteroidSystemModel = createAsteroidSystemModel();

    glUseProgram(modelShaderProgram);
    configureModelPointShadows();
    configureModelLighting(
        modelUniforms,
        view,
        projection,
        cameraPos,
        cameraFront,
        lightPos,
        mainLightColor,
        secondLightPos,
        secondLightColor,
        false,
        false,
        true,
        false
    );
    glUniform1i(modelUniforms.pointShadowsEnabled, false);
    glUniform1i(modelUniforms.glassBackpackEnabled, false);
    glUniform1f(uniformLocation(modelShaderProgram, "material.shininess"), 16.0f);
    setMatrix(modelUniforms.model, planetModel);
    planet->draw(modelShaderProgram);

    glUseProgram(instancedModelShaderProgram);
    const glm::mat4 lightSpaceMatrix = createDirectionalLightSpaceMatrix(sunDirection());
    setMatrix(uniformLocation(instancedModelShaderProgram, "lightSpaceMatrix"), lightSpaceMatrix);
    bindTexture(GL_TEXTURE3, GL_TEXTURE_2D, directionalDepthMap);
    glUniform1i(uniformLocation(instancedModelShaderProgram, "directionalShadowMap"), 3);
    setMatrix(uniformLocation(instancedModelShaderProgram, "systemModel"), asteroidSystemModel);
    configureInstancedLighting(
        instancedModelShaderProgram,
        view,
        projection,
        cameraPos,
        cameraFront,
        lightPos,
        mainLightColor,
        secondLightPos,
        secondLightColor,
        false,
        false,
        true,
        false);

    setVector(uniformLocation(instancedModelShaderProgram, "objectColor"), glm::vec3(0.55f, 0.45f, 0.34f));
    setVector(uniformLocation(instancedModelShaderProgram, "objectSpecularColor"), glm::vec3(0.08f));
    glUniform1f(uniformLocation(instancedModelShaderProgram, "shininess"), 8.0f);
    rock->drawInstanced(instancedModelShaderProgram, ASTEROID_COUNT);
}

void Week09Scene::createDirectionalShadowMap()
{
    glGenFramebuffers(1, &directionalDepthFBO);
    glGenTextures(1, &directionalDepthMap);
    glBindTexture(GL_TEXTURE_2D, directionalDepthMap);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT24,
        SHADOW_WIDTH,
        SHADOW_HEIGHT,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, directionalDepthFBO);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        directionalDepthMap,
        0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Directional shadow framebuffer incomplete\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 Week09Scene::createDirectionalLightSpaceMatrix(const glm::vec3& direction) const
{
    const glm::vec3 sceneCenter(0.0f);
    const glm::vec3 lightPosition = sceneCenter - glm::normalize(direction) * 10.0f;
    const glm::mat4 lightProjection = glm::ortho(-7.0f, 7.0f, -7.0f, 7.0f, 0.1f, 25.0f);
    const glm::mat4 lightView = glm::lookAt(
        lightPosition,
        sceneCenter,
        glm::vec3(0.0f, 1.0f, 0.0f));
    return lightProjection * lightView;
}

void Week09Scene::drawDirectionalShadowCasters(
    GLuint depthShaderProgram,
    const glm::mat4& lightSpaceMatrix)
{
    (void)lightSpaceMatrix;
    const GLint modelLocation = uniformLocation(depthShaderProgram, "model");
    drawCube(cube.VAO, modelLocation, createOutdoorGroundModelMatrix());
    drawCube(
        cube.VAO,
        modelLocation,
        makeTransform(glm::vec3(-0.2f, -0.6f, 0.0f), glm::vec3(1.0f)));
    drawSphere(
        sphere,
        modelLocation,
        makeTransform(glm::vec3(0.8f, -0.4f, 0.0f), glm::vec3(1.0f)));

    if (backpack)
    {
        setMatrix(modelLocation, createBackpackModelMatrix());
        backpack->draw(depthShaderProgram);
    }

    if (planet)
    {
        const glm::mat4 planetModel = makeTransform(
            glm::vec3(1.3f, 0.0f, -1.5f),
            glm::vec3(0.018f * 8.0f));
        setMatrix(modelLocation, planetModel);
        planet->draw(depthShaderProgram);
    }

    if (rock)
    {
        glUseProgram(instancedDirectionalDepthShaderProgram);
        setMatrix(
            uniformLocation(instancedDirectionalDepthShaderProgram, "lightSpaceMatrix"),
            createDirectionalLightSpaceMatrix(sunDirection()));
        setMatrix(
            uniformLocation(instancedDirectionalDepthShaderProgram, "systemModel"),
            createAsteroidSystemModel());
        rock->drawInstanced(instancedDirectionalDepthShaderProgram, ASTEROID_COUNT);
        glUseProgram(depthShaderProgram);
    }
}

void Week09Scene::renderDirectionalShadowMap(const glm::mat4& lightSpaceMatrix)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, directionalDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(directionalDepthShaderProgram);
    setMatrix(uniformLocation(directionalDepthShaderProgram, "lightSpaceMatrix"), lightSpaceMatrix);
    glCullFace(GL_FRONT);
    drawDirectionalShadowCasters(directionalDepthShaderProgram, lightSpaceMatrix);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 Week09Scene::createAsteroidSystemModel() const
{
    const float orbitAngle =
        static_cast<float>(glfwGetTime()) *
        glm::radians(3.0f);

    glm::mat4 model = glm::translate(
        glm::mat4(1.0f),
        DISTANT_PLANET_POSITION
    );

    model = glm::rotate(
        model,
        glm::radians(0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    model = glm::rotate(
        model,
        orbitAngle,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return glm::scale(
        model,
        glm::vec3(DISTANT_ASTEROID_SYSTEM_SCALE)
    );
}

void Week09Scene::createPointShadowMap()
{
    glGenFramebuffers(1, &pointDepthFBO);
    glGenTextures(1, &pointDepthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthCubemap);

    for (unsigned int face = 0; face < 6; ++face)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
            0,
            GL_DEPTH_COMPONENT24,
            POINT_SHADOW_WIDTH,
            POINT_SHADOW_HEIGHT,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, pointDepthFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Point shadow framebuffer incomplete\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::vector<glm::mat4> Week09Scene::createPointShadowMatrices(
    const glm::vec3& lightPosition) const
{
    const float aspect = static_cast<float>(POINT_SHADOW_WIDTH) /
        static_cast<float>(POINT_SHADOW_HEIGHT);
    const glm::mat4 projection = glm::perspective(
        glm::radians(90.0f),
        aspect,
        POINT_SHADOW_NEAR_PLANE,
        POINT_SHADOW_FAR_PLANE);

    const std::array<glm::vec3, 6> directions = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    const std::array<glm::vec3, 6> upVectors = {
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f)
    };

    std::vector<glm::mat4> matrices;
    matrices.reserve(6);
    for (std::size_t face = 0; face < directions.size(); ++face)
    {
        matrices.push_back(projection * glm::lookAt(
            lightPosition,
            lightPosition + directions[face],
            upVectors[face]));
    }
    return matrices;
}

void Week09Scene::drawPointShadowCasters(GLuint depthShaderProgram)
{
    const GLint modelLocation = uniformLocation(depthShaderProgram, "model");
    drawCube(
        cube.VAO,
        modelLocation,
        makeTransform(glm::vec3(-0.2f, -0.6f, 0.0f), glm::vec3(1.0f)));
    drawSphere(
        sphere,
        modelLocation,
        makeTransform(glm::vec3(0.8f, -0.4f, 0.0f), glm::vec3(1.0f)));

    if (backpack)
    {
        setMatrix(modelLocation, createBackpackModelMatrix());
        backpack->draw(depthShaderProgram);
    }

    if (planet)
    {
        setMatrix(
            modelLocation,
            makeTransform(glm::vec3(1.3f, 0.0f, -1.5f), glm::vec3(0.018f * 8.0f)));
        planet->draw(depthShaderProgram);
    }
}

void Week09Scene::renderPointShadowMap(const glm::vec3& lightPosition)
{
    const std::vector<glm::mat4> shadowMatrices = createPointShadowMatrices(lightPosition);
    glViewport(0, 0, POINT_SHADOW_WIDTH, POINT_SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, pointDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(pointDepthShaderProgram);

    for (unsigned int face = 0; face < 6; ++face)
    {
        const std::string uniformName = "shadowMatrices[" + std::to_string(face) + "]";
        setMatrix(uniformLocation(pointDepthShaderProgram, uniformName.c_str()), shadowMatrices[face]);
    }

    setVector(uniformLocation(pointDepthShaderProgram, "lightPos"), lightPosition);
    glUniform1f(uniformLocation(pointDepthShaderProgram, "farPlane"), POINT_SHADOW_FAR_PLANE);

    const GLboolean cullingWasEnabled = glIsEnabled(GL_CULL_FACE);
    GLint previousCullFaceMode = GL_BACK;
    glGetIntegerv(GL_CULL_FACE_MODE, &previousCullFaceMode);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    drawPointShadowCasters(pointDepthShaderProgram);
    glCullFace(static_cast<GLenum>(previousCullFaceMode));
    if (!cullingWasEnabled)
    {
        glDisable(GL_CULL_FACE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Week09Scene::configureModelPointShadows()
{
    bindTexture(GL_TEXTURE4, GL_TEXTURE_CUBE_MAP, pointDepthCubemap);
    glUniform1i(modelUniforms.pointShadowMap, 4);
    glUniform1f(modelUniforms.pointShadowFarPlane, POINT_SHADOW_FAR_PLANE);
    glUniform1i(modelUniforms.pointShadowsEnabled, !outdoorModeEnabled && light1Enabled);
}

void Week09Scene::createNormalMappedPlane()
{
    const glm::vec3 p1(-1.0f, 1.0f, 0.0f);
    const glm::vec3 p2(1.0f, 1.0f, 0.0f);
    const glm::vec3 p3(1.0f, -1.0f, 0.0f);
    const glm::vec3 p4(-1.0f, -1.0f, 0.0f);
    const glm::vec2 uv1(0.0f, 1.0f);
    const glm::vec2 uv2(1.0f, 1.0f);
    const glm::vec2 uv3(1.0f, 0.0f);
    const glm::vec2 uv4(0.0f, 0.0f);
    const glm::vec3 normal(0.0f, 0.0f, 1.0f);
    const TangentBasis first = calculateTangentBasis(p1, p2, p3, uv1, uv2, uv3);
    const TangentBasis second = calculateTangentBasis(p1, p3, p4, uv1, uv3, uv4);

    const float vertices[] = {
        p1.x, p1.y, p1.z, normal.x, normal.y, normal.z, uv1.x, uv1.y, first.tangent.x, first.tangent.y, first.tangent.z, first.bitangent.x, first.bitangent.y, first.bitangent.z,
        p2.x, p2.y, p2.z, normal.x, normal.y, normal.z, uv2.x, uv2.y, first.tangent.x, first.tangent.y, first.tangent.z, first.bitangent.x, first.bitangent.y, first.bitangent.z,
        p3.x, p3.y, p3.z, normal.x, normal.y, normal.z, uv3.x, uv3.y, first.tangent.x, first.tangent.y, first.tangent.z, first.bitangent.x, first.bitangent.y, first.bitangent.z,
        p1.x, p1.y, p1.z, normal.x, normal.y, normal.z, uv1.x, uv1.y, second.tangent.x, second.tangent.y, second.tangent.z, second.bitangent.x, second.bitangent.y, second.bitangent.z,
        p3.x, p3.y, p3.z, normal.x, normal.y, normal.z, uv3.x, uv3.y, second.tangent.x, second.tangent.y, second.tangent.z, second.bitangent.x, second.bitangent.y, second.bitangent.z,
        p4.x, p4.y, p4.z, normal.x, normal.y, normal.z, uv4.x, uv4.y, second.tangent.x, second.tangent.y, second.tangent.z, second.bitangent.x, second.bitangent.y, second.bitangent.z
    };

    glGenVertexArrays(1, &normalMappedPlaneVAO);
    glGenBuffers(1, &normalMappedPlaneVBO);
    glBindVertexArray(normalMappedPlaneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalMappedPlaneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    constexpr GLsizei stride = 14 * sizeof(float);
    configureVertexAttribute(0, 3, stride, 0);
    configureVertexAttribute(1, 3, stride, 3 * sizeof(float));
    configureVertexAttribute(2, 2, stride, 6 * sizeof(float));
    configureVertexAttribute(3, 3, stride, 8 * sizeof(float));
    configureVertexAttribute(4, 3, stride, 11 * sizeof(float));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Week09Scene::drawNormalMappedSurface(
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
    const glm::vec3& lightColor)
{
    glUseProgram(normalMappingShaderProgram);
    setMatrix(normalMappingUniforms.model, model);
    setMatrix(normalMappingUniforms.view, view);
    setMatrix(normalMappingUniforms.projection, projection);
    setVector(normalMappingUniforms.viewPos, cameraPos);
    setVector(normalMappingUniforms.lightPos, lightPos);
    setVector(normalMappingUniforms.lightColor, lightColor);
    glUniform1i(normalMappingUniforms.lightEnabled, !outdoorModeEnabled && light1Enabled);
    setVector(normalMappingUniforms.wallLightPos,WALL_LIGHT_POSITION);
    setVector(normalMappingUniforms.wallLightColor, WALL_LIGHT_COLOR);
    glUniform1i(normalMappingUniforms.wallLightEnabled, !outdoorModeEnabled && light1Enabled);
    glUniform2f(normalMappingUniforms.uvScale, uvScale.x, uvScale.y);
    glUniform1f(normalMappingUniforms.normalStrength, normalStrength);
    glUniform1f(normalMappingUniforms.heightScale,heightScale);
    bindTexture(GL_TEXTURE0, GL_TEXTURE_2D, diffuseTexture);
    bindTexture(GL_TEXTURE1, GL_TEXTURE_2D, normalTexture);
    bindTexture(GL_TEXTURE2, GL_TEXTURE_2D, depthTexture);

    const GLboolean cullingWasEnabled = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glBindVertexArray(normalMappedPlaneVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    if (cullingWasEnabled)
    {
        glEnable(GL_CULL_FACE);
    }
    glActiveTexture(GL_TEXTURE0);
}

void Week09Scene::drawNormalMappedRoomWalls(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& lightPos,
    const glm::vec3& lightColor)
{
    struct Wall
    {
        glm::vec3 position;
        float rotation;
    };

    constexpr float roomWallPosition = 2.95f;

    const std::array<Wall, 3> walls = {
        Wall{ glm::vec3(0.0f, 1.0f, roomWallPosition), 180.0f },
        Wall{ glm::vec3(-roomWallPosition, 1.0f, 0.0f), 90.0f },
        Wall{ glm::vec3(roomWallPosition, 1.0f, 0.0f), -90.0f }
    };

    for (const Wall& wall : walls)
    {
        drawNormalMappedSurface(
            makeTransform(
                wall.position,
                glm::vec3(3.0f, 2.0f, 1.0f),
                wall.rotation
            ),
            glm::vec2(4.0f, 3.0f),
            brickDiffuseTexture,
            brickNormalTexture,
            brickDepthTexture,
            1.0f,
            parallaxHeightScale,
            view,
            projection,
            cameraPos,
            lightPos,
            lightColor
        );
    }
}

void Week09Scene::drawNormalMappedFloorAndCeiling(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& lightPos,
    const glm::vec3& lightColor)
{
    struct Surface
    {
        glm::vec3 position;
        float rotation;
        glm::vec2 uvScale;
        float normalStrength;
    };

    const std::array<Surface, 2> surfaces = {
        Surface{ glm::vec3(0.0f, -0.95f, 0.0f), -90.0f, glm::vec2(3.0f), 0.9f },
        Surface{ glm::vec3(0.0f, 2.95f, 0.0f), 90.0f, glm::vec2(2.0f), 0.3f }
    };

    for (const Surface& surface : surfaces)
    {
        drawNormalMappedSurface(
            makeTransform(
                surface.position,
                glm::vec3(3.0f, 3.0f, 1.0f),
                surface.rotation,
                glm::vec3(1.0f, 0.0f, 0.0f)
            ),
            surface.uvScale,
            brickDiffuseTexture,
            brickNormalTexture,
            brickDepthTexture,
            surface.normalStrength,
            parallaxHeightScale,
            view,
            projection,
            cameraPos,
            lightPos,
            lightColor
        );
    }
}

glm::mat4 Week09Scene::createWallLampModelMatrix() const
{
    return makeTransform(
        WALL_LAMP_POSITION,
        glm::vec3(1.0f),
        -90.0f,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

void Week09Scene::cleanup()
{
    deleteCubeMesh(cube);
    deleteSphereMesh(sphere);

    const GLuint textures[] = {
        containerTexture,
        containerSpecularTexture,
        glassTexture,
        framebufferTexture,
        multisampleColorTexture,
        directionalDepthMap,
        pointDepthCubemap,
        brickDiffuseTexture,
        brickNormalTexture,
        brickDepthTexture,
        floorDiffuseTexture,
        floorNormalTexture,
        ceilingDiffuseTexture,
        ceilingNormalTexture,
        pingPongTextures[0],
        pingPongTextures[1]
    };
    glDeleteTextures(static_cast<GLsizei>(sizeof(textures) / sizeof(textures[0])), textures);

    const GLuint programs[] = {
        shaderProgram,
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
        gaussianBlurShaderProgram
    };
    for (GLuint program : programs)
    {
        glDeleteProgram(program);
    }

    const GLuint framebuffers[] = {
        framebuffer,
        multisampleFramebuffer,
        directionalDepthFBO,
        pointDepthFBO,
        pingPongFBO[0],
        pingPongFBO[1]
    };
    glDeleteFramebuffers(static_cast<GLsizei>(sizeof(framebuffers) / sizeof(framebuffers[0])), framebuffers);

    const GLuint vertexArrays[] = { glassVAO, screenQuadVAO, normalMappedPlaneVAO };
    glDeleteVertexArrays(static_cast<GLsizei>(sizeof(vertexArrays) / sizeof(vertexArrays[0])), vertexArrays);

    const GLuint buffers[] = { glassVBO, screenQuadVBO, asteroidInstanceVBO, normalMappedPlaneVBO };
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
    delete wallLamp;
    wallLamp = nullptr;
}
