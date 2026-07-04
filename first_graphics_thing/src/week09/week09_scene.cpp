#include "week09_scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/shader.h"
#include "../common/texture.h"

#include <map>
#include <vector>

#include <iostream>

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

    containerTexture = loadTexture("assets/textures/container.png");
    containerSpecularTexture = loadTexture("assets/textures/container_specular.png");

    glassTexture = loadTransparentTexture("assets/textures/window.png");

    backpack = new Model("assets/models/backpack/backpack.obj");

    cube = createCubeMesh();
    sphere = createSphereMesh();

    createGlassPaneMesh();

    createFramebuffer(width, height);
    createScreenQuad();

    glassPanePositions.push_back(glm::vec3(-1.8f, 0.0f, -1.8f));
    glassPanePositions.push_back(glm::vec3(0.0f, 0.0f, -1.8f));
    glassPanePositions.push_back(glm::vec3(1.8f, 0.0f, -1.8f));

    glassPanePositions.push_back(glm::vec3(-1.2f, 0.0f, 0.8f));
    glassPanePositions.push_back(glm::vec3(1.2f, 0.0f, 0.8f));

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

    glUseProgram(glassShaderProgram);
    glUniform1i(glGetUniformLocation(glassShaderProgram, "glassTexture"), 0);

    glUseProgram(modelShaderProgram);
    glUniform1f(
        glGetUniformLocation(modelShaderProgram, "material.shininess"),
        32.0f
    );

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

    keyBWasPressed = keyBIsPressed;
    keyVWasPressed = keyVIsPressed;
    keyFWasPressed = keyFIsPressed;
    keyNWasPressed = keyNIsPressed;
    key1WasPressed = key1IsPressed;
    key2WasPressed = key2IsPressed;
    keyPWasPressed = keyPIsPressed;
    keyGWasPressed = keyGIsPressed;
    keyMWasPressed = keyMIsPressed;
}

void Week09Scene::draw(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& movableLightPos,
    const glm::vec3& cameraFront
)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::vec3 lightPos = movableLightPos;
    glm::vec3 lightColor(1.25f, 1.25f, 1.25f);

    glm::vec3 secondLightPos(-2.0f, 2.2f, -2.0f);
    glm::vec3 secondLightColor(0.3f, 0.5f, 1.0f);


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

    glUniform1i(
        lightingUniforms.light1Enabled,
        !outdoorModeEnabled && light1Enabled
    );

    glUniform1i(
        lightingUniforms.light2Enabled,
        !outdoorModeEnabled && light2Enabled
    );

    glm::vec3 sunDirection(-0.3f, -1.0f, -0.2f);
    glm::vec3 sunColor(1.0f, 0.95f, 0.80f);

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

    // Second pass: draw framebuffer texture to the real screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(screenShaderProgram);

    glUniform1i(
        glGetUniformLocation(screenShaderProgram, "blurEnabled"),
        blurEnabled
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);

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

    if (!outdoorModeEnabled)
    {
        // room only in indoor mode
        setMaterial(shaderProgram, matteGray);
        drawClosedRoom(
            cube.VAO,
            lightingUniforms.model,
            containerTexture
        );
    }
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
        lightingUniforms.light1Enabled, !outdoorModeEnabled && light1Enabled
    );

    glUniform1i(
        lightingUniforms.light2Enabled, !outdoorModeEnabled && light2Enabled
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
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
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

    glGenRenderbuffers(1, &framebufferRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, framebufferRBO);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        width,
        height
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        framebufferRBO
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);

    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
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

    glDeleteTextures(1, &glassTexture);
    glDeleteVertexArrays(1, &glassVAO);
    glDeleteBuffers(1, &glassVBO);

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &framebufferTexture);
    glDeleteRenderbuffers(1, &framebufferRBO);

    glDeleteVertexArrays(1, &screenQuadVAO);
    glDeleteBuffers(1, &screenQuadVBO);

    glDeleteProgram(screenShaderProgram);

    delete backpack;
    backpack = nullptr;

    delete skybox;
    skybox = nullptr;
}