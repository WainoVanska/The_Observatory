#include "week09_scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/shader.h"
#include "../common/texture.h"


void Week09Scene::init()
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
        "src/week13/model_vertex.glsl",
        "src/week13/model_fragment.glsl"
    );

    containerTexture = loadTexture("assets/textures/container.png");
    containerSpecularTexture = loadTexture("assets/textures/container_specular.png");

    backpack = new Model("assets/models/backpack/backpack.obj");

    cube = createCubeMesh();
    sphere = createSphereMesh();

    dullOrange = createDullOrangeMaterial();
    shinyBlue = createShinyBlueMaterial();
    matteGray = createMatteGrayMaterial();
    texturedMaterial = createTexturedMaterial();

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

    lampUniforms.model = glGetUniformLocation(lampShaderProgram, "model");
    lampUniforms.view = glGetUniformLocation(lampShaderProgram, "view");
    lampUniforms.projection = glGetUniformLocation(lampShaderProgram, "projection");
    lampUniforms.color = glGetUniformLocation(lampShaderProgram, "lampColor");

    normalDebugUniforms.model = glGetUniformLocation(normalDebugShaderProgram, "model");
    normalDebugUniforms.view = glGetUniformLocation(normalDebugShaderProgram, "view");
    normalDebugUniforms.projection = glGetUniformLocation(normalDebugShaderProgram, "projection");

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

    glUseProgram(modelShaderProgram);

    glUniform1f(
        glGetUniformLocation(modelShaderProgram, "material.shininess"),
        32.0f
    );
}

void Week09Scene::handleInput(GLFWwindow* window)
{
    bool key1IsPressed = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
    bool key2IsPressed = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
    bool keyNIsPressed = glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;

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
    }

    bool keyFIsPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;

    if (keyFIsPressed && !keyFWasPressed)
    {
        flashlightEnabled = !flashlightEnabled;
    }

    keyFWasPressed = keyFIsPressed;
    keyNWasPressed = keyNIsPressed;
    key1WasPressed = key1IsPressed;
    key2WasPressed = key2IsPressed;
}

void Week09Scene::draw(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& movableLightPos,
    const glm::vec3& cameraFront
)
{
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

        return;
    }

    // Draw lit objects
    glUseProgram(shaderProgram);

    glUniform1i(lightingUniforms.light1Enabled, light1Enabled);
    glUniform1i(lightingUniforms.light2Enabled, light2Enabled);

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

    drawLitObjects();

    drawBackpack(
        view,
        projection,
        cameraPos,
        lightPos
    );

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

    if (light1Enabled)
    {
        drawLampMarker(lightPos, glm::vec3(1.0f, 1.0f, 0.3f));
    }

    if (light2Enabled)
    {
        drawLampMarker(secondLightPos, glm::vec3(0.3f, 0.5f, 1.0f));
    }

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

    // room
    setMaterial(shaderProgram, matteGray);
    drawClosedRoom(
        cube.VAO,
        lightingUniforms.model,
        containerTexture
    );
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
    const glm::vec3& lightPos
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

    glm::mat4 backpackModel = glm::mat4(1.0f);

    // Put it in front of the camera / inside the room
    backpackModel = glm::translate(
        backpackModel,
        glm::vec3(0.0f, -1.0f, -2.0f)
    );

    // Rotate it so we see the front/back instead of the thin side
    backpackModel = glm::rotate(
        backpackModel,
        glm::radians(90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Make it much smaller
    backpackModel = glm::scale(
        backpackModel,
        glm::vec3(0.03f)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(modelShaderProgram, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(backpackModel)
    );

    backpack->draw(modelShaderProgram);
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
    glDeleteProgram(modelShaderProgram);

    delete backpack;
    backpack = nullptr;
}