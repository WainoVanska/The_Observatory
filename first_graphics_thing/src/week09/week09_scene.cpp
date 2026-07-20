#include "week09_scene.h"

#include "../common/shader.h"
#include "../common/texture.h"

#include <map>
#include <vector>
#include <iostream>
#include <random>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void Week09Scene::init(int width, int height, FreeCamera* camera)
{
    shaderProgram = createShaderProgram(
        "src/week09/vertex.glsl",
        "src/week09/fragment.glsl"
    );

    lampShaderProgram = createShaderProgram(
        "src/week09/lamp_vertex.glsl",
        "src/week09/lamp_fragment.glsl"
    );

    normalDebugShaderProgram = createShaderProgram(
        "src/week09/vertex.glsl",
        "src/week09/normal_debug_fragment.glsl"
    );

    modelShaderProgram = createShaderProgram(
        "src/week09/model_vertex.glsl",
        "src/week09/model_fragment.glsl"
    );

    modelNormalDebugShaderProgram = createShaderProgram(
        "src/week09/model_vertex.glsl",
        "src/week09/normal_debug_fragment.glsl"
    );

    depthDebugShaderProgram = createShaderProgram(
        "src/week09/vertex.glsl",
        "src/week09/depth_debug_fragment.glsl"
    );

    modelDepthDebugShaderProgram = createShaderProgram(
        "src/week09/model_vertex.glsl",
        "src/week09/depth_debug_fragment.glsl"
    );

    modelOutlineShaderProgram = createShaderProgram(
        "src/week09/model_vertex.glsl",
        "src/week09/single_color_fragment.glsl"
    );

    glassShaderProgram = createShaderProgram(
        "src/week09/glass_vertex.glsl",
        "src/week09/glass_fragment.glsl"
    );

    screenShaderProgram = createShaderProgram(
        "src/week09/screen_vertex.glsl",
        "src/week09/screen_fragment.glsl"
    );

    instancedModelShaderProgram = createShaderProgram(
        "src/week09/instanced_model_vertex.glsl",
        "src/week09/instanced_rock_fragment.glsl"
    );

    directionalDepthShaderProgram =
        createShaderProgram(
            "src/week09/directional_depth_vertex.glsl",
            "src/week09/directional_depth_fragment.glsl"
        );

    instancedDirectionalDepthShaderProgram =
        createShaderProgram(
            "src/week09/instanced_directional_depth_vertex.glsl",
            "src/week09/directional_depth_fragment.glsl"
        );

    pointDepthShaderProgram =
        createShaderProgram(
            "src/week09/point_depth_vertex.glsl",
            "src/week09/point_depth_geometry.glsl",
            "src/week09/point_depth_fragment.glsl"
        );

    // sRGB
    containerTexture = loadTexture("assets/textures/container.png", true);

    // not sRGB
    containerSpecularTexture = loadTexture("assets/textures/container_specular.png", false);

    // Visible RGB is sRGB, alpha is unaffected by sRGB
    glassTexture = loadTransparentTexture("assets/textures/window.png", true);

    backpack = new Model("assets/models/backpack/backpack.obj");
    planet = new Model("assets/models/planet/planet.obj");
    rock = new Model("assets/models/rock/rock.obj");

    cube = createCubeMesh();
    sphere = createSphereMesh();

    createAsteroidInstances();
    createGlassPaneMesh();
    createFramebuffer(width, height);
    createScreenQuad();
    createDirectionalShadowMap();
    createPointShadowMap();

    glassPanePositions.push_back(glm::vec3(-1.8f, 0.0f, -1.8f));
    glassPanePositions.push_back(glm::vec3(0.0f, 0.0f, -1.8f));
    glassPanePositions.push_back(glm::vec3(1.8f, 0.0f, -1.8f));

    glassPanePositions.push_back(glm::vec3(-1.2f, 0.0f, 0.8f));
    glassPanePositions.push_back(glm::vec3(1.2f, 0.0f, 0.8f));

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
        "assets/textures/skybox1",
        "px.png",
        "nx.png",
        "py.png",
        "ny.png",
        "pz.png",
        "nz.png"
    )) {
        std::cout << "Skybox failed to load\n";
    }

    lightingUniforms.model = glGetUniformLocation(shaderProgram, "model");
    lightingUniforms.view = glGetUniformLocation(shaderProgram, "view");
    lightingUniforms.projection = glGetUniformLocation(shaderProgram, "projection");
    lightingUniforms.viewPos = glGetUniformLocation(shaderProgram, "viewPos");

    lightingUniforms.lightPos = glGetUniformLocation(shaderProgram, "lightPos");
    lightingUniforms.lightColor = glGetUniformLocation(shaderProgram, "lightColor");
    lightingUniforms.secondLightPos = glGetUniformLocation(shaderProgram, "secondLightPos");
    lightingUniforms.secondLightColor = glGetUniformLocation(shaderProgram, "secondLightColor");

    lightingUniforms.light1Enabled = glGetUniformLocation(shaderProgram, "light1Enabled");
    lightingUniforms.light2Enabled = glGetUniformLocation(shaderProgram, "light2Enabled");

    lightingUniforms.directionalLightEnabled = glGetUniformLocation(shaderProgram, "directionalLightEnabled");
    lightingUniforms.directionalLightDirection = glGetUniformLocation(shaderProgram, "directionalLightDirection");
    lightingUniforms.directionalLightColor = glGetUniformLocation(shaderProgram, "directionalLightColor");

    lightingUniforms.lightSpaceMatrix = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
    lightingUniforms.directionalShadowMap = glGetUniformLocation(shaderProgram, "directionalShadowMap");

    glUseProgram(shaderProgram);
    glUniform1i(lightingUniforms.directionalShadowMap, 3);

    lampUniforms.model = glGetUniformLocation(lampShaderProgram, "model");
    lampUniforms.view = glGetUniformLocation(lampShaderProgram, "view");
    lampUniforms.projection = glGetUniformLocation(lampShaderProgram, "projection");
    lampUniforms.color = glGetUniformLocation(lampShaderProgram, "lampColor");

    normalDebugUniforms.model = glGetUniformLocation(normalDebugShaderProgram, "model");
    normalDebugUniforms.view = glGetUniformLocation(normalDebugShaderProgram, "view");
    normalDebugUniforms.projection = glGetUniformLocation(normalDebugShaderProgram, "projection");

    modelNormalDebugUniforms.model = glGetUniformLocation(modelNormalDebugShaderProgram, "model");
    modelNormalDebugUniforms.view = glGetUniformLocation(modelNormalDebugShaderProgram, "view");
    modelNormalDebugUniforms.projection = glGetUniformLocation(modelNormalDebugShaderProgram, "projection");

    depthDebugUniforms.model = glGetUniformLocation(depthDebugShaderProgram, "model");
    depthDebugUniforms.view = glGetUniformLocation(depthDebugShaderProgram, "view");
    depthDebugUniforms.projection = glGetUniformLocation(depthDebugShaderProgram, "projection");
    depthDebugUniforms.nearPlane = glGetUniformLocation(depthDebugShaderProgram, "nearPlane");
    depthDebugUniforms.farPlane = glGetUniformLocation(depthDebugShaderProgram, "farPlane");

    modelDepthDebugUniforms.model = glGetUniformLocation(modelDepthDebugShaderProgram, "model");
    modelDepthDebugUniforms.view = glGetUniformLocation(modelDepthDebugShaderProgram, "view");
    modelDepthDebugUniforms.projection = glGetUniformLocation(modelDepthDebugShaderProgram, "projection");
    modelDepthDebugUniforms.nearPlane = glGetUniformLocation(modelDepthDebugShaderProgram, "nearPlane");
    modelDepthDebugUniforms.farPlane = glGetUniformLocation(modelDepthDebugShaderProgram, "farPlane");

    lightingUniforms.flashlightPos = glGetUniformLocation(shaderProgram, "flashlightPos");
    lightingUniforms.flashlightDir = glGetUniformLocation(shaderProgram, "flashlightDir");
    lightingUniforms.flashlightEnabled = glGetUniformLocation(shaderProgram, "flashlightEnabled");
    lightingUniforms.flashlightCutOff = glGetUniformLocation(shaderProgram, "flashlightCutOff");
    lightingUniforms.flashlightOuterCutOff = glGetUniformLocation(shaderProgram, "flashlightOuterCutOff");

    modelUniforms.model = glGetUniformLocation(modelShaderProgram, "model");
    modelUniforms.view = glGetUniformLocation(modelShaderProgram, "view");
    modelUniforms.projection = glGetUniformLocation(modelShaderProgram, "projection");
    modelUniforms.viewPos = glGetUniformLocation(modelShaderProgram, "viewPos");
    modelUniforms.lightPos = glGetUniformLocation(modelShaderProgram, "lightPos");
    modelUniforms.lightColor = glGetUniformLocation(modelShaderProgram, "lightColor");

    modelUniforms.secondLightPos = glGetUniformLocation(modelShaderProgram, "secondLightPos");
    modelUniforms.secondLightColor = glGetUniformLocation(modelShaderProgram, "secondLightColor");

    modelUniforms.light1Enabled = glGetUniformLocation(modelShaderProgram, "light1Enabled");
    modelUniforms.light2Enabled = glGetUniformLocation(modelShaderProgram, "light2Enabled");

    modelUniforms.directionalLightEnabled = glGetUniformLocation(modelShaderProgram, "directionalLightEnabled");
    modelUniforms.directionalLightDirection = glGetUniformLocation(modelShaderProgram, "directionalLightDirection");
    modelUniforms.directionalLightColor = glGetUniformLocation(modelShaderProgram, "directionalLightColor");

    modelUniforms.skybox = glGetUniformLocation(modelShaderProgram, "skybox");
    modelUniforms.glassBackpackEnabled = glGetUniformLocation(modelShaderProgram, "glassBackpackEnabled");

    modelUniforms.flashlightPos = glGetUniformLocation(modelShaderProgram, "flashlightPos");
    modelUniforms.flashlightDir = glGetUniformLocation(modelShaderProgram, "flashlightDir");
    modelUniforms.flashlightEnabled = glGetUniformLocation(modelShaderProgram, "flashlightEnabled");
    modelUniforms.flashlightCutOff = glGetUniformLocation(modelShaderProgram, "flashlightCutOff");
    modelUniforms.flashlightOuterCutOff = glGetUniformLocation(modelShaderProgram, "flashlightOuterCutOff");

    modelOutlineUniforms.model = glGetUniformLocation(modelOutlineShaderProgram, "model");
    modelOutlineUniforms.view = glGetUniformLocation(modelOutlineShaderProgram, "view");
    modelOutlineUniforms.projection = glGetUniformLocation(modelOutlineShaderProgram, "projection");

    glassUniforms.model = glGetUniformLocation(glassShaderProgram, "model");
    glassUniforms.view = glGetUniformLocation(glassShaderProgram, "view");
    glassUniforms.projection = glGetUniformLocation(glassShaderProgram, "projection");

    lightingUniforms.pointShadowMap = glGetUniformLocation(shaderProgram,"pointShadowMap");
    lightingUniforms.pointShadowFarPlane =glGetUniformLocation(shaderProgram,"pointShadowFarPlane");
    lightingUniforms.pointShadowsEnabled =glGetUniformLocation(shaderProgram,"pointShadowsEnabled");

    glUseProgram(shaderProgram);

    glUniform1i(
        lightingUniforms.pointShadowMap,
        4
    );

    glUseProgram(glassShaderProgram);
    glUniform1i(glGetUniformLocation(glassShaderProgram, "glassTexture"), 0);

    glUseProgram(modelShaderProgram);
    glUniform1f(
        glGetUniformLocation(modelShaderProgram, "material.shininess"),
        32.0f
    );

    glUniform1i(modelUniforms.skybox, 2);

    glUseProgram(screenShaderProgram);
    screenTextureUniform = glGetUniformLocation(screenShaderProgram, "screenTexture");
    glUniform1i(screenTextureUniform, 0);
}

void Week09Scene::handleInput(GLFWwindow* window)
{
    bool key1IsPressed = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
    bool key2IsPressed = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
    bool keyNIsPressed = glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;
    bool keyVIsPressed = glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS;
    bool keyPIsPressed = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
    bool keyGIsPressed = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
    bool keyFIsPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    bool keyBIsPressed = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
    bool keyMIsPressed = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
    bool keyHIsPressed = glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS;

    if (key1IsPressed && !key1WasPressed)
    {
        light1Enabled = !light1Enabled;
    }

    if (key2IsPressed && !key2WasPressed)
    {
        light2Enabled = !light2Enabled;
    }


    if (keyNIsPressed && !keyNWasPressed)
    {
        normalDebugEnabled = !normalDebugEnabled;

        if (normalDebugEnabled)
        {
            depthDebugEnabled = false;
        }
    }

    if (keyFIsPressed && !keyFWasPressed)
    {
        flashlightEnabled = !flashlightEnabled;
    }

    if (keyVIsPressed && !keyVWasPressed)
    {
        depthDebugEnabled = !depthDebugEnabled;

        if (depthDebugEnabled)
        {
            normalDebugEnabled = false;
        }
    }

    if (keyPIsPressed && !keyPWasPressed)
    {
        outlineEnabled = !outlineEnabled;
    }

    if (keyGIsPressed && !keyGWasPressed)
    {
        glassEnabled = !glassEnabled;
    }

    if (keyBIsPressed && !keyBWasPressed)
    {
        blurEnabled = !blurEnabled;
    }

    if (keyMIsPressed && !keyMWasPressed)
    {
        outdoorModeEnabled = !outdoorModeEnabled;
    }

    if (keyHIsPressed && !keyHWasPressed)
    {
        glassBackpackEnabled = !glassBackpackEnabled;
    }

    keyBWasPressed = keyBIsPressed;
    keyVWasPressed = keyVIsPressed;
    keyFWasPressed = keyFIsPressed;
    keyNWasPressed = keyNIsPressed;
    key1WasPressed = key1IsPressed;
    key2WasPressed = key2IsPressed;
    keyPWasPressed = keyPIsPressed;
    keyGWasPressed = keyGIsPressed;
    keyMWasPressed = keyMIsPressed;
    keyHWasPressed = keyHIsPressed;
}

void Week09Scene::draw(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& movableLightPos,
    const glm::vec3& cameraFront
)
{
    const glm::vec3 sunDirection(
        -0.3f,
        -1.0f,
        -0.2f
    );

    const glm::vec3 sunColor(
        1.0f,
        0.95f,
        0.80f
    );

    const glm::vec3 lightPos =
        movableLightPos;

    glm::mat4 lightSpaceMatrix(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if (outdoorModeEnabled)
    {
        lightSpaceMatrix =
            createDirectionalLightSpaceMatrix(
                sunDirection
            );

        renderDirectionalShadowMap(
            lightSpaceMatrix
        );
    }
    else if (light1Enabled)
    {
        renderPointShadowMap(
            lightPos
        );
    }

    glViewport(
        0,
        0,
        framebufferWidth,
        framebufferHeight
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        multisampleFramebuffer
    );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(
        0.08f,
        0.08f,
        0.12f,
        1.0f
    );

    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT |
        GL_STENCIL_BUFFER_BIT
    );

    const glm::vec3 lightColor(
        0.70f,
        0.63f,
        0.50f
    );

    glm::vec3 secondLightPos(
        -2.0f,
        2.2f,
        -2.0f
    );

    glm::vec3 secondLightColor(
        0.3f,
        0.5f,
        1.0f
    );


    if (normalDebugEnabled)
    {
        glUseProgram(normalDebugShaderProgram);

        glUniformMatrix4fv(
            normalDebugUniforms.view,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            normalDebugUniforms.projection,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        drawNormalDebugObjects();

        drawBackpackNormalDebug(
            view,
            projection
        );
    }
    else if (depthDebugEnabled)
    {
        glUseProgram(depthDebugShaderProgram);

        glUniformMatrix4fv(
            depthDebugUniforms.view,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            depthDebugUniforms.projection,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        glUniform1f(depthDebugUniforms.nearPlane, 0.1f);
        glUniform1f(depthDebugUniforms.farPlane, 100.0f);

        drawDepthDebugObjects();

        drawBackpackDepthDebug(
            view,
            projection
        );
    }

    // Draw lit objects
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(
        lightingUniforms.lightSpaceMatrix,
        1,
        GL_FALSE,
        glm::value_ptr(lightSpaceMatrix)
    );

    //directional shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(
        GL_TEXTURE_2D,
        directionalDepthMap
    );

    // First point-light shadow cubemap
    glActiveTexture(GL_TEXTURE4);

    glBindTexture(
        GL_TEXTURE_CUBE_MAP,
        pointDepthCubemap
    );

    glUniform1f(
        lightingUniforms.pointShadowFarPlane,
        POINT_SHADOW_FAR_PLANE
    );

    glUniform1i(
        lightingUniforms.pointShadowsEnabled,
        !outdoorModeEnabled &&
        light1Enabled
    );

    glUniform1i(
        lightingUniforms.light1Enabled,
        !outdoorModeEnabled && light1Enabled
    );

    glUniform1i(
        lightingUniforms.light2Enabled,
        !outdoorModeEnabled && light2Enabled
    );

    glUniform1i(
        lightingUniforms.directionalLightEnabled,
        outdoorModeEnabled
    );

    glUniform3fv(
        lightingUniforms.directionalLightDirection,
        1,
        glm::value_ptr(sunDirection)
    );

    glUniform3fv(
        lightingUniforms.directionalLightColor,
        1,
        glm::value_ptr(sunColor)
    );

    glUniformMatrix4fv(
        lightingUniforms.view,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        lightingUniforms.projection,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glUniform3fv(
        lightingUniforms.viewPos,
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        lightingUniforms.lightPos,
        1,
        glm::value_ptr(lightPos)
    );

    glUniform3fv(
        lightingUniforms.lightColor,
        1,
        glm::value_ptr(lightColor)
    );

    glUniform3fv(
        lightingUniforms.secondLightPos,
        1,
        glm::value_ptr(secondLightPos)
    );

    glUniform3fv(
        lightingUniforms.secondLightColor,
        1,
        glm::value_ptr(secondLightColor)
    );

    // draw flashlight uniforms
    glUniform3fv(
        lightingUniforms.flashlightPos,
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        lightingUniforms.flashlightDir,
        1,
        glm::value_ptr(cameraFront)
    );

    glUniform1i(
        lightingUniforms.flashlightEnabled,
        flashlightEnabled
    );

    glUniform1f(
        lightingUniforms.flashlightCutOff,
        glm::cos(glm::radians(10.0f))
    );

    glUniform1f(
        lightingUniforms.flashlightOuterCutOff,
        glm::cos(glm::radians(28.0f))
    );

    // Do not write the room/cube/sphere into stencil
    glStencilMask(0x00);

    drawLitObjects();

    drawMiniPlanetSystem(
        view,
        projection,
        cameraPos,
        cameraFront,
        lightPos,
        secondLightPos,
        secondLightColor
    );

    if (outlineEnabled)
    {
        // 1. Draw backpack normally and write value 1 into stencil.
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        drawBackpack(
            view,
            projection,
            cameraPos,
            cameraFront,
            lightPos,
            secondLightPos,
            secondLightColor
        );

        // 2. Draw scaled-up backpack only where stencil is NOT 1.
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        drawBackpackOutline(
            view,
            projection
        );

        // 3. Restore normal stencil state.
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
    }
    else
    {
        // No outline: draw backpack normally, no stencil writing.
        glStencilMask(0x00);

        drawBackpack(
            view,
            projection,
            cameraPos,
            cameraFront,
            lightPos,
            secondLightPos,
            secondLightColor
        );

        glStencilMask(0xFF);
    }

    glStencilMask(0x00);

    // Draw lamp markers
    glUseProgram(lampShaderProgram);

    glUniformMatrix4fv(
        lampUniforms.view,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        lampUniforms.projection,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    if (!outdoorModeEnabled)
    {
        if (light1Enabled)
        {
            drawLampMarker(lightPos, glm::vec3(1.0f, 1.0f, 0.3f));
        }

        if (light2Enabled)
        {
            drawLampMarker(secondLightPos, glm::vec3(0.3f, 0.5f, 1.0f));
        }
    }

    glStencilMask(0xFF);

    if (glassEnabled)
    {
        drawGlassPanes(
            view,
            projection,
            cameraPos
        );
    }

    if (outdoorModeEnabled && skybox)
    {
        skybox->Render(projection);
    }

    // Resolve multisampled framebuffer into the normal framebuffer texture.
    // The screen shader samples framebufferTexture as a normal sampler2D,
    // so we cannot directly sample the multisampled texture.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

    glBlitFramebuffer(
        0, 0, framebufferWidth, framebufferHeight,
        0, 0, framebufferWidth, framebufferHeight,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );

    // Second pass: draw resolved framebuffer texture to the real screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(screenShaderProgram);

    glUniform1i(
        glGetUniformLocation(
            screenShaderProgram,
            "blurEnabled"
        ),
        blurEnabled
    );

    glUniform1i(
        glGetUniformLocation(
            screenShaderProgram,
            "gammaCorrectionEnabled"
        ),
        true
    );

    glUniform1i(
        glGetUniformLocation(
            screenShaderProgram,
            "hdrEnabled"
        ),
        true
    );

    glUniform1f(
        glGetUniformLocation(
            screenShaderProgram,
            "exposure"
        ),
        1.2f
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(
        GL_TEXTURE_2D,
        framebufferTexture
    );

    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Restore state for next frame
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
}

void Week09Scene::drawLitObjects()
{
    // test cube
    glm::mat4 testCube = glm::mat4(1.0f);
    testCube = glm::translate(testCube, glm::vec3(-0.2f, -0.6f, 0.0f));

    setMaterial(shaderProgram, texturedMaterial);
    drawTexturedCubeWithSpecularMap(
        cube.VAO,
        lightingUniforms.model,
        testCube,
        containerTexture,
        containerSpecularTexture
    );

    // test sphere
    glm::mat4 testSphere = glm::mat4(1.0f);
    testSphere = glm::translate(testSphere, glm::vec3(0.8f, -0.4f, 0.0f));

    setMaterial(shaderProgram, shinyBlue);
    drawSphere(
        sphere,
        lightingUniforms.model,
        testSphere
    );

    if (outdoorModeEnabled)
    {
        setMaterial(
            shaderProgram,
            outdoorGroundMaterial
        );

        drawCube(
            cube.VAO,
            lightingUniforms.model,
            createOutdoorGroundModelMatrix()
        );
    }
    else
    {
        setMaterial(
            shaderProgram,
            matteGray
        );

        drawClosedRoom(
            cube.VAO,
            lightingUniforms.model,
            containerTexture
        );
    }
}

glm::mat4 Week09Scene::createOutdoorGroundModelMatrix() const
{
    glm::mat4 model(1.0f);

    model = glm::translate(
        model,
        glm::vec3(0.0f, -0.95f, 0.0f)
    );

    model = glm::scale(
        model,
        glm::vec3(20.0f, 0.1f, 20.0f)
    );

    return model;
}

void Week09Scene::drawNormalDebugObjects()
{
    // test cube
    glm::mat4 testCube = glm::mat4(1.0f);
    testCube = glm::translate(testCube, glm::vec3(-0.2f, -0.6f, 0.0f));

    drawTexturedCube(
        cube.VAO,
        normalDebugUniforms.model,
        testCube,
        containerTexture
    );

    // test sphere
    glm::mat4 testSphere = glm::mat4(1.0f);
    testSphere = glm::translate(testSphere, glm::vec3(0.8f, -0.4f, 0.0f));

    drawSphere(
        sphere,
        normalDebugUniforms.model,
        testSphere
    );

    // room
    drawClosedRoom(
        cube.VAO,
        normalDebugUniforms.model,
        containerTexture
    );
}

void Week09Scene::drawDepthDebugObjects()
{
    // test cube
    glm::mat4 testCube = glm::mat4(1.0f);
    testCube = glm::translate(testCube, glm::vec3(-0.2f, -0.6f, 0.0f));

    drawTexturedCube(
        cube.VAO,
        depthDebugUniforms.model,
        testCube,
        containerTexture
    );

    // test sphere
    glm::mat4 testSphere = glm::mat4(1.0f);
    testSphere = glm::translate(testSphere, glm::vec3(0.8f, -0.4f, 0.0f));

    drawSphere(
        sphere,
        depthDebugUniforms.model,
        testSphere
    );

    // room
    drawClosedRoom(
        cube.VAO,
        depthDebugUniforms.model,
        containerTexture
    );
}

void Week09Scene::drawLampMarker(
    const glm::vec3& position,
    const glm::vec3& color
)
{
    glm::mat4 lampModel = glm::mat4(1.0f);
    lampModel = glm::translate(lampModel, position);
    lampModel = glm::scale(lampModel, glm::vec3(0.15f));

    glUniform3fv(
        lampUniforms.color,
        1,
        glm::value_ptr(color)
    );

    drawCube(
        cube.VAO,
        lampUniforms.model,
        lampModel
    );
}

void Week09Scene::drawBackpack(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& cameraFront,
    const glm::vec3& lightPos,
    const glm::vec3& secondLightPos,
    const glm::vec3& secondLightColor
)
{
    if (!backpack)
    {
        return;
    }

    glUseProgram(modelShaderProgram);

    glUniform1i(
        modelUniforms.glassBackpackEnabled,
        glassBackpackEnabled
    );

    if (skybox)
    {
        skybox->BindTexture(GL_TEXTURE2);
    }

    glUniformMatrix4fv(
        modelUniforms.view,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        modelUniforms.projection,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glUniform3fv(
        modelUniforms.viewPos,
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        modelUniforms.lightPos,
        1,
        glm::value_ptr(lightPos)
    );

    glm::vec3 modelLightColor(1.0f, 1.0f, 1.0f);

    glUniform3fv(
        modelUniforms.lightColor,
        1,
        glm::value_ptr(modelLightColor)
    );

    glUniform3fv(
        modelUniforms.secondLightPos,
        1,
        glm::value_ptr(secondLightPos)
    );

    glUniform3fv(
        modelUniforms.secondLightColor,
        1,
        glm::value_ptr(secondLightColor)
    );

    glUniform1i(
        modelUniforms.light1Enabled,
        !outdoorModeEnabled && light1Enabled
    );

    glUniform1i(
        modelUniforms.light2Enabled,
        !outdoorModeEnabled && light2Enabled
    );

    glm::vec3 sunDirection(-0.3f, -1.0f, -0.2f);
    glm::vec3 sunColor(1.0f, 0.95f, 0.80f);

    glUniform1i(
        modelUniforms.directionalLightEnabled,
        outdoorModeEnabled
    );

    glUniform3fv(
        modelUniforms.directionalLightDirection,
        1,
        glm::value_ptr(sunDirection)
    );

    glUniform3fv(
        modelUniforms.directionalLightColor,
        1,
        glm::value_ptr(sunColor)
    );

    glUniform3fv(
        modelUniforms.flashlightPos,
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        modelUniforms.flashlightDir,
        1,
        glm::value_ptr(cameraFront)
    );

    glUniform1i(
        modelUniforms.flashlightEnabled,
        flashlightEnabled
    );

    glUniform1f(
        modelUniforms.flashlightCutOff,
        glm::cos(glm::radians(10.0f))
    );

    glUniform1f(
        modelUniforms.flashlightOuterCutOff,
        glm::cos(glm::radians(28.0f))
    );

    glm::mat4 backpackModel = createBackpackModelMatrix();

    glUniformMatrix4fv(
        modelUniforms.model,
        1,
        GL_FALSE,
        glm::value_ptr(backpackModel)
    );

    backpack->draw(modelShaderProgram);
}

glm::mat4 Week09Scene::createBackpackOutlineModelMatrix() const
{
    glm::mat4 backpackModel = glm::mat4(1.0f);

    backpackModel = glm::translate(
        backpackModel,
        glm::vec3(-1.0f, -0.5f, 0.0f)
    );

    backpackModel = glm::rotate(
        backpackModel,
        glm::radians(90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    backpackModel = glm::scale(
        backpackModel,
        glm::vec3(0.215f)
    );

    return backpackModel;
}

void Week09Scene::drawBackpackNormalDebug(
    const glm::mat4& view,
    const glm::mat4& projection
)
{
    if (!backpack)
    {
        return;
    }

    glUseProgram(modelNormalDebugShaderProgram);

    glUniformMatrix4fv(
        modelNormalDebugUniforms.view,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        modelNormalDebugUniforms.projection,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glm::mat4 backpackModel = createBackpackModelMatrix();

    glUniformMatrix4fv(
        modelNormalDebugUniforms.model,
        1,
        GL_FALSE,
        glm::value_ptr(backpackModel)
    );

    backpack->draw(modelNormalDebugShaderProgram);
}

void Week09Scene::drawBackpackOutline(
    const glm::mat4& view,
    const glm::mat4& projection
)
{
    if (!backpack)
    {
        return;
    }

    glUseProgram(modelOutlineShaderProgram);

    glUniformMatrix4fv(
        modelOutlineUniforms.view,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        modelOutlineUniforms.projection,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glm::mat4 backpackModel = createBackpackOutlineModelMatrix();

    glUniformMatrix4fv(
        modelOutlineUniforms.model,
        1,
        GL_FALSE,
        glm::value_ptr(backpackModel)
    );

    backpack->draw(modelOutlineShaderProgram);
}

void Week09Scene::drawBackpackDepthDebug(
    const glm::mat4& view,
    const glm::mat4& projection
)
{
    if (!backpack)
    {
        return;
    }

    glUseProgram(modelDepthDebugShaderProgram);

    glUniformMatrix4fv(
        modelDepthDebugUniforms.view,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        modelDepthDebugUniforms.projection,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glUniform1f(modelDepthDebugUniforms.nearPlane, 0.1f);
    glUniform1f(modelDepthDebugUniforms.farPlane, 100.0f);

    glm::mat4 backpackModel = createBackpackModelMatrix();

    glUniformMatrix4fv(
        modelDepthDebugUniforms.model,
        1,
        GL_FALSE,
        glm::value_ptr(backpackModel)
    );

    backpack->draw(modelDepthDebugShaderProgram);
}

glm::mat4 Week09Scene::createBackpackModelMatrix() const
{
    glm::mat4 backpackModel = glm::mat4(1.0f);

    backpackModel = glm::translate(
        backpackModel,
        glm::vec3(-1.0f, -0.5f, 0.0f)
    );

    backpackModel = glm::rotate(
        backpackModel,
        glm::radians(90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    backpackModel = glm::scale(
        backpackModel,
        glm::vec3(0.205f)
    );

    return backpackModel;
}

void Week09Scene::createGlassPaneMesh()
{
    float vertices[] = {
        // positions          // UVs
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,

        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f
    };

    glGenVertexArrays(1, &glassVAO);
    glGenBuffers(1, &glassVBO);

    glBindVertexArray(glassVAO);

    glBindBuffer(GL_ARRAY_BUFFER, glassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)0
    );

    // texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glBindVertexArray(0);
}

void Week09Scene::drawGlassPanes(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos
)
{
    std::map<float, glm::vec3> sorted;

    for (const glm::vec3& position : glassPanePositions)
    {
        float distance = glm::length(cameraPos - position);

        // Tiny offset avoids overwriting if two distances are exactly equal.
        while (sorted.find(distance) != sorted.end())
        {
            distance += 0.0001f;
        }

        sorted[distance] = position;
    }

    glUseProgram(glassShaderProgram);

    glUniformMatrix4fv(
        glassUniforms.view,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        glassUniforms.projection,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glassTexture);

    glBindVertexArray(glassVAO);

    // Transparent objects should not write depth, otherwise front panes can hide back panes.
    glDepthMask(GL_FALSE);

    // Do not let transparent panes affect stencil/outline.
    glStencilMask(0x00);

    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, it->second);

        // Put the pane vertically: local quad is XY plane already, facing ±Z.
        model = glm::scale(model, glm::vec3(1.0f, 1.4f, 1.0f));

        glUniformMatrix4fv(
            glassUniforms.model,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glStencilMask(0xFF);
}

void Week09Scene::createFramebuffer(int width, int height)
{
    framebufferWidth = width;
    framebufferHeight = height;

    // Multisampled framebuffer

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
        GL_TRUE
    );

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE,
        multisampleColorTexture,
        0
    );

    glGenRenderbuffers(1, &multisampleRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, multisampleRBO);

    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER,
        MSAA_SAMPLES,
        GL_DEPTH24_STENCIL8,
        width,
        height
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        multisampleRBO
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Multisample framebuffer is not complete!" << std::endl;
    }

    // Normal resolved framebuffer

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
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        framebufferTexture,
        0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Resolved framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Week09Scene::createScreenQuad()
{
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &screenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);

    glBindVertexArray(screenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quadVertices),
        quadVertices,
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );

    glBindVertexArray(0);
}

void Week09Scene::drawScreenQuad()
{
    glUseProgram(screenShaderProgram);

    glUniform1i(
        glGetUniformLocation(screenShaderProgram, "blurEnabled"),
        blurEnabled
    );

    glUniform1i(
        glGetUniformLocation(
            screenShaderProgram,
            "gammaCorrectionEnabled"
        ),
        true
    );

    glUniform1i(
        glGetUniformLocation(
            screenShaderProgram,
            "hdrEnabled"
        ),
        true
    );

    glUniform1f(
        glGetUniformLocation(
            screenShaderProgram,
            "exposure"
        ),
        1.2f
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);

    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Week09Scene::createAsteroidInstances()
{
    if (!rock)
    {
        return;
    }

    asteroidMatrices.resize(ASTEROID_COUNT);

    std::mt19937 randomEngine(1337);

    std::uniform_real_distribution<float> displacement(
        -8.0f,
        8.0f
    );

    std::uniform_real_distribution<float> scaleDistribution(
        0.10f,
        0.28f
    );

    std::uniform_real_distribution<float> rotationDistribution(
        0.0f,
        glm::two_pi<float>()
    );

    constexpr float radius = 50.0f;

    for (unsigned int i = 0; i < ASTEROID_COUNT; i++)
    {
        float percentage =
            static_cast<float>(i) /
            static_cast<float>(ASTEROID_COUNT);

        float angle =
            percentage *
            glm::two_pi<float>();

        float x =
            std::sin(angle) * radius +
            displacement(randomEngine);

        float y =
            displacement(randomEngine) * 0.25f;

        float z =
            std::cos(angle) * radius +
            displacement(randomEngine);

        glm::mat4 model(1.0f);

        model = glm::translate(
            model,
            glm::vec3(x, y, z)
        );

        float scale =
            scaleDistribution(randomEngine);

        model = glm::scale(
            model,
            glm::vec3(scale)
        );

        model = glm::rotate(
            model,
            rotationDistribution(randomEngine),
            glm::normalize(
                glm::vec3(0.4f, 0.6f, 0.8f)
            )
        );

        asteroidMatrices[i] = model;
    }

    glGenBuffers(1, &asteroidInstanceVBO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        asteroidInstanceVBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        asteroidMatrices.size() * sizeof(glm::mat4),
        asteroidMatrices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    rock->configureInstancing(
        asteroidInstanceVBO
    );
}

void Week09Scene::drawMiniPlanetSystem(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& cameraFront,
    const glm::vec3& lightPos,
    const glm::vec3& secondLightPos,
    const glm::vec3& secondLightColor
)
{
    if (!planet || !rock)
    {
        return;
    }

    const glm::vec3 mainLightColor(1.25f, 1.25f, 1.25f);

    glm::mat4 baseSystemModel(1.0f);

    baseSystemModel = glm::translate(
        baseSystemModel,
        glm::vec3(1.3f, 0.0f, -1.5f)
    );

    baseSystemModel = glm::rotate(
        baseSystemModel,
        glm::radians(0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    // Planet parent transform
    glm::mat4 planetSystemModel =
        glm::scale(
            baseSystemModel,
            glm::vec3(0.018f)
        );

    // Asteroid parent transform
    glm::mat4 asteroidSystemModel =
        createAsteroidSystemModel();

    glUseProgram(modelShaderProgram);

    glm::mat4 planetLocal(1.0f);

    planetLocal = glm::scale(
        planetLocal,
        glm::vec3(8.0f)
    );

    glm::mat4 planetModel =
        planetSystemModel * planetLocal;

    glUniformMatrix4fv(
        modelUniforms.model,
        1,
        GL_FALSE,
        glm::value_ptr(planetModel)
    );

    glUniformMatrix4fv(
        modelUniforms.view,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        modelUniforms.projection,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glUniform3fv(
        modelUniforms.viewPos,
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        modelUniforms.lightPos,
        1,
        glm::value_ptr(lightPos)
    );

    glUniform3fv(
        modelUniforms.lightColor,
        1,
        glm::value_ptr(mainLightColor)
    );

    glUniform3fv(
        modelUniforms.secondLightPos,
        1,
        glm::value_ptr(secondLightPos)
    );

    glUniform3fv(
        modelUniforms.secondLightColor,
        1,
        glm::value_ptr(secondLightColor)
    );

    glUniform1i(
        modelUniforms.light1Enabled,
        !outdoorModeEnabled && light1Enabled
    );

    glUniform1i(
        modelUniforms.light2Enabled,
        !outdoorModeEnabled && light2Enabled
    );

    glm::vec3 sunDirection(-0.3f, -1.0f, -0.2f);
    glm::vec3 sunColor(1.0f, 0.95f, 0.80f);

    glUniform1i(
        modelUniforms.directionalLightEnabled,
        outdoorModeEnabled
    );

    glUniform3fv(
        modelUniforms.directionalLightDirection,
        1,
        glm::value_ptr(sunDirection)
    );

    glUniform3fv(
        modelUniforms.directionalLightColor,
        1,
        glm::value_ptr(sunColor)
    );

    glUniform3fv(
        modelUniforms.flashlightPos,
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        modelUniforms.flashlightDir,
        1,
        glm::value_ptr(cameraFront)
    );

    glUniform1i(
        modelUniforms.flashlightEnabled,
        flashlightEnabled
    );

    glUniform1f(
        modelUniforms.flashlightCutOff,
        glm::cos(glm::radians(10.0f))
    );

    glUniform1f(
        modelUniforms.flashlightOuterCutOff,
        glm::cos(glm::radians(28.0f))
    );

    // Ensure the planet is not rendered using backpack refraction.
    glUniform1i(
        modelUniforms.glassBackpackEnabled,
        false
    );

    glUniform1f(
        glGetUniformLocation(
            modelShaderProgram,
            "material.shininess"
        ),
        16.0f
    );

    planet->draw(modelShaderProgram);

    // Draw the instanced asteroid ring

    glUseProgram(instancedModelShaderProgram);

    const glm::mat4 lightSpaceMatrix =
        createDirectionalLightSpaceMatrix(
            sunDirection
        );

    glUniformMatrix4fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "lightSpaceMatrix"
        ),
        1,
        GL_FALSE,
        glm::value_ptr(lightSpaceMatrix)
    );

    glActiveTexture(GL_TEXTURE3);

    glBindTexture(
        GL_TEXTURE_2D,
        directionalDepthMap
    );

    glUniform1i(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "directionalShadowMap"
        ),
        3
    );

    glUniformMatrix4fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "systemModel"
        ),
        1,
        GL_FALSE,
        glm::value_ptr(asteroidSystemModel)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "view"
        ),
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "projection"
        ),
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "viewPos"
        ),
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "lightPos"
        ),
        1,
        glm::value_ptr(lightPos)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "lightColor"
        ),
        1,
        glm::value_ptr(mainLightColor)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "secondLightPos"
        ),
        1,
        glm::value_ptr(secondLightPos)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "secondLightColor"
        ),
        1,
        glm::value_ptr(secondLightColor)
    );

    glUniform1i(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "light1Enabled"
        ),
        !outdoorModeEnabled && light1Enabled
    );

    glUniform1i(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "light2Enabled"
        ),
        !outdoorModeEnabled && light2Enabled
    );

    glUniform1i(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "directionalLightEnabled"
        ),
        outdoorModeEnabled
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "directionalLightDirection"
        ),
        1,
        glm::value_ptr(sunDirection)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "directionalLightColor"
        ),
        1,
        glm::value_ptr(sunColor)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "flashlightPos"
        ),
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "flashlightDir"
        ),
        1,
        glm::value_ptr(cameraFront)
    );

    glUniform1i(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "flashlightEnabled"
        ),
        flashlightEnabled
    );

    glUniform1f(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "flashlightCutOff"
        ),
        glm::cos(glm::radians(10.0f))
    );

    glUniform1f(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "flashlightOuterCutOff"
        ),
        glm::cos(glm::radians(28.0f))
    );

    const glm::vec3 rockColor(
        0.55f,
        0.45f,
        0.34f
    );

    const glm::vec3 rockSpecularColor(
        0.08f,
        0.08f,
        0.08f
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "objectColor"
        ),
        1,
        glm::value_ptr(rockColor)
    );

    glUniform3fv(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "objectSpecularColor"
        ),
        1,
        glm::value_ptr(rockSpecularColor)
    );

    glUniform1f(
        glGetUniformLocation(
            instancedModelShaderProgram,
            "shininess"
        ),
        8.0f
    );

    rock->drawInstanced(
        instancedModelShaderProgram,
        ASTEROID_COUNT
    );

}

void Week09Scene::createDirectionalShadowMap()
{
    glGenFramebuffers(
        1,
        &directionalDepthFBO
    );

    glGenTextures(
        1,
        &directionalDepthMap
    );

    glBindTexture(
        GL_TEXTURE_2D,
        directionalDepthMap
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT24,
        SHADOW_WIDTH,
        SHADOW_HEIGHT,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_BORDER
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_BORDER
    );

    const float borderColor[] = {
        1.0f,
        1.0f,
        1.0f,
        1.0f
    };

    glTexParameterfv(
        GL_TEXTURE_2D,
        GL_TEXTURE_BORDER_COLOR,
        borderColor
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        directionalDepthFBO
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        directionalDepthMap,
        0
    );

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (
        glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE
        )
    {
        std::cerr
            << "Directional shadow framebuffer incomplete\n";
    }

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

glm::mat4 Week09Scene::createDirectionalLightSpaceMatrix(
    const glm::vec3& sunDirection
) const
{
    const glm::vec3 sceneCenter(
        0.0f,
        0.0f,
        0.0f
    );

    const glm::vec3 lightPosition =
        sceneCenter -
        glm::normalize(sunDirection) *
        10.0f;

    const glm::mat4 lightProjection =
        glm::ortho(
            -7.0f,
            7.0f,
            -7.0f,
            7.0f,
            0.1f,
            25.0f
        );

    const glm::mat4 lightView =
        glm::lookAt(
            lightPosition,
            sceneCenter,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

    return lightProjection * lightView;
}

void Week09Scene::drawDirectionalShadowCasters(
    GLuint depthShaderProgram,
    const glm::mat4& lightSpaceMatrix
)
{
    const GLint modelLocation =
        glGetUniformLocation(
            depthShaderProgram,
            "model"
        );

    // Outdoor ground
    drawCube(
        cube.VAO,
        modelLocation,
        createOutdoorGroundModelMatrix()
    );

    // Test cube
    glm::mat4 cubeModel(1.0f);

    cubeModel = glm::translate(
        cubeModel,
        glm::vec3(-0.2f, -0.6f, 0.0f)
    );

    drawCube(
        cube.VAO,
        modelLocation,
        cubeModel
    );

    // Sphere
    glm::mat4 sphereModel(1.0f);

    sphereModel = glm::translate(
        sphereModel,
        glm::vec3(0.8f, -0.4f, 0.0f)
    );

    drawSphere(
        sphere,
        modelLocation,
        sphereModel
    );

    // Backpack
    if (backpack)
    {
        const glm::mat4 backpackModel =
            createBackpackModelMatrix();

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(backpackModel)
        );

        backpack->draw(
            depthShaderProgram
        );
    }

    // Planet
    if (planet)
    {
        glm::mat4 planetModel(1.0f);

        planetModel = glm::translate(
            planetModel,
            glm::vec3(1.3f, 0.0f, -1.5f)
        );

        planetModel = glm::scale(
            planetModel,
            glm::vec3(0.018f)
        );

        planetModel = glm::scale(
            planetModel,
            glm::vec3(8.0f)
        );

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(planetModel)
        );

        planet->draw(
            depthShaderProgram
        );
    }

    // Instanced asteroid ring
    if (rock)
    {
        glUseProgram(
            instancedDirectionalDepthShaderProgram
        );

        glUniformMatrix4fv(
            glGetUniformLocation(
                instancedDirectionalDepthShaderProgram,
                "lightSpaceMatrix"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                createDirectionalLightSpaceMatrix(
                    glm::vec3(-0.3f, -1.0f, -0.2f)
                )
            )
        );

        const glm::mat4 asteroidSystemModel =
            createAsteroidSystemModel();

        glUniformMatrix4fv(
            glGetUniformLocation(
                instancedDirectionalDepthShaderProgram,
                "systemModel"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(asteroidSystemModel)
        );

        rock->drawInstanced(
            instancedDirectionalDepthShaderProgram,
            ASTEROID_COUNT
        );

        glUseProgram(depthShaderProgram);
    }
}

void Week09Scene::renderDirectionalShadowMap(
    const glm::mat4& lightSpaceMatrix
)
{
    glViewport(
        0,
        0,
        SHADOW_WIDTH,
        SHADOW_HEIGHT
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        directionalDepthFBO
    );

    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(
        directionalDepthShaderProgram
    );

    glUniformMatrix4fv(
        glGetUniformLocation(
            directionalDepthShaderProgram,
            "lightSpaceMatrix"
        ),
        1,
        GL_FALSE,
        glm::value_ptr(lightSpaceMatrix)
    );

    glCullFace(GL_FRONT);

    drawDirectionalShadowCasters(
        directionalDepthShaderProgram,
        lightSpaceMatrix
    );

    glCullFace(GL_BACK);

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

glm::mat4 Week09Scene::createAsteroidSystemModel() const
{
    const float orbitAngle =
        static_cast<float>(glfwGetTime()) *
        glm::radians(3.0f);

    glm::mat4 systemModel(1.0f);

    systemModel = glm::translate(
        systemModel,
        glm::vec3(1.3f, 0.0f, -1.5f)
    );

    systemModel = glm::rotate(
        systemModel,
        orbitAngle,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    systemModel = glm::scale(
        systemModel,
        glm::vec3(0.018f)
    );

    return systemModel;
}

void Week09Scene::createPointShadowMap()
{
    glGenFramebuffers(
        1,
        &pointDepthFBO
    );

    glGenTextures(
        1,
        &pointDepthCubemap
    );

    glBindTexture(
        GL_TEXTURE_CUBE_MAP,
        pointDepthCubemap
    );

    for (unsigned int face = 0;
        face < 6;
        ++face)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X +
            face,
            0,
            GL_DEPTH_COMPONENT24,
            POINT_SHADOW_WIDTH,
            POINT_SHADOW_HEIGHT,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr
        );
    }

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_R,
        GL_CLAMP_TO_EDGE
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        pointDepthFBO
    );

    // Attach the cubemap
    glFramebufferTexture(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        pointDepthCubemap,
        0
    );

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (
        glCheckFramebufferStatus(
            GL_FRAMEBUFFER
        ) != GL_FRAMEBUFFER_COMPLETE
        )
    {
        std::cerr
            << "Point shadow framebuffer incomplete\n";
    }

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

std::vector<glm::mat4>
Week09Scene::createPointShadowMatrices(
    const glm::vec3& lightPosition
) const
{
    const float aspect =
        static_cast<float>(
            POINT_SHADOW_WIDTH
            ) /
        static_cast<float>(
            POINT_SHADOW_HEIGHT
            );

    const glm::mat4 shadowProjection =
        glm::perspective(
            glm::radians(90.0f),
            aspect,
            POINT_SHADOW_NEAR_PLANE,
            POINT_SHADOW_FAR_PLANE
        );

    std::vector<glm::mat4>
        shadowMatrices;

    shadowMatrices.reserve(6);

    shadowMatrices.push_back(
        shadowProjection *
        glm::lookAt(
            lightPosition,
            lightPosition +
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)
        )
    );

    shadowMatrices.push_back(
        shadowProjection *
        glm::lookAt(
            lightPosition,
            lightPosition +
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)
        )
    );

    shadowMatrices.push_back(
        shadowProjection *
        glm::lookAt(
            lightPosition,
            lightPosition +
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        )
    );

    shadowMatrices.push_back(
        shadowProjection *
        glm::lookAt(
            lightPosition,
            lightPosition +
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)
        )
    );

    shadowMatrices.push_back(
        shadowProjection *
        glm::lookAt(
            lightPosition,
            lightPosition +
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)
        )
    );

    shadowMatrices.push_back(
        shadowProjection *
        glm::lookAt(
            lightPosition,
            lightPosition +
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)
        )
    );

    return shadowMatrices;
}

void Week09Scene::drawPointShadowCasters(
    GLuint depthShaderProgram
)
{
    const GLint modelLocation =
        glGetUniformLocation(
            depthShaderProgram,
            "model"
        );

    drawClosedRoom(
        cube.VAO,
        modelLocation,
        containerTexture
    );

    glm::mat4 cubeModel(1.0f);

    cubeModel = glm::translate(
        cubeModel,
        glm::vec3(-0.2f, -0.6f, 0.0f)
    );

    drawCube(
        cube.VAO,
        modelLocation,
        cubeModel
    );

    glm::mat4 sphereModel(1.0f);

    sphereModel = glm::translate(
        sphereModel,
        glm::vec3(0.8f, -0.4f, 0.0f)
    );

    drawSphere(
        sphere,
        modelLocation,
        sphereModel
    );

    if (backpack)
    {
        const glm::mat4 backpackModel =
            createBackpackModelMatrix();

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(backpackModel)
        );

        backpack->draw(
            depthShaderProgram
        );
    }

    if (planet)
    {
        glm::mat4 planetModel(1.0f);

        planetModel = glm::translate(
            planetModel,
            glm::vec3(1.3f, 0.0f, -1.5f)
        );

        planetModel = glm::scale(
            planetModel,
            glm::vec3(0.018f)
        );

        planetModel = glm::scale(
            planetModel,
            glm::vec3(8.0f)
        );

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(planetModel)
        );

        planet->draw(
            depthShaderProgram
        );
    }
}

void Week09Scene::renderPointShadowMap(
    const glm::vec3& lightPosition
)
{
    const std::vector<glm::mat4>
        shadowMatrices =
        createPointShadowMatrices(
            lightPosition
        );

    glViewport(
        0,
        0,
        POINT_SHADOW_WIDTH,
        POINT_SHADOW_HEIGHT
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        pointDepthFBO
    );

    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(
        pointDepthShaderProgram
    );

    for (unsigned int face = 0;
        face < 6;
        ++face)
    {
        const std::string uniformName =
            "shadowMatrices[" +
            std::to_string(face) +
            "]";

        glUniformMatrix4fv(
            glGetUniformLocation(
                pointDepthShaderProgram,
                uniformName.c_str()
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                shadowMatrices[face]
            )
        );
    }

    glUniform3fv(
        glGetUniformLocation(
            pointDepthShaderProgram,
            "lightPos"
        ),
        1,
        glm::value_ptr(lightPosition)
    );

    glUniform1f(
        glGetUniformLocation(
            pointDepthShaderProgram,
            "farPlane"
        ),
        POINT_SHADOW_FAR_PLANE
    );

    //make culling disabled since we need the back faces not shown also for shadows
    const GLboolean cullingWasEnabled =
        glIsEnabled(GL_CULL_FACE);

    glDisable(GL_CULL_FACE);

    drawPointShadowCasters(
        pointDepthShaderProgram
    );

    if (cullingWasEnabled)
    {
        glEnable(GL_CULL_FACE);
    }

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

void Week09Scene::cleanup()
{
    deleteCubeMesh(cube);
    deleteSphereMesh(sphere);

    glDeleteTextures(1, &containerTexture);
    glDeleteTextures(1, &containerSpecularTexture);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(lampShaderProgram);
    glDeleteProgram(normalDebugShaderProgram);
    glDeleteProgram(modelNormalDebugShaderProgram);
    glDeleteProgram(modelShaderProgram);
    glDeleteProgram(depthDebugShaderProgram);
    glDeleteProgram(modelDepthDebugShaderProgram);
    glDeleteProgram(modelOutlineShaderProgram);
    glDeleteProgram(glassShaderProgram);
    glDeleteProgram(instancedDirectionalDepthShaderProgram);

    glDeleteTextures(1, &glassTexture);
    glDeleteVertexArrays(1, &glassVAO);
    glDeleteBuffers(1, &glassVBO);

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &framebufferTexture);

    glDeleteFramebuffers(1, &multisampleFramebuffer);
    glDeleteTextures(1, &multisampleColorTexture);
    glDeleteRenderbuffers(1, &multisampleRBO);

    glDeleteVertexArrays(1, &screenQuadVAO);
    glDeleteBuffers(1, &screenQuadVBO);

    glDeleteBuffers(1, &asteroidInstanceVBO);
    glDeleteProgram(instancedModelShaderProgram);

    glDeleteProgram(screenShaderProgram);

    glDeleteProgram(directionalDepthShaderProgram);

    glDeleteFramebuffers(1, &directionalDepthFBO);
    glDeleteTextures(1, &directionalDepthMap);

    delete backpack;
    backpack = nullptr;

    delete skybox;
    skybox = nullptr;

    delete planet;
    planet = nullptr;

    delete rock;
    rock = nullptr;
}