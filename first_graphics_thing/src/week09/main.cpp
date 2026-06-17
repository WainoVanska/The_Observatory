#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "../common/cube.h"
#include "../common/free_camera.h"
#include "../common/texture.h"
#include "../common/sphere.h"
#include "../common/light_controller.h"
#include "../common/shader.h"
#include "../common/window.h"
#include "../common/material.h"

int main()
{
    // GLFW init
    const int windowWidth = 800;
    const int windowHeight = 600;

    GLFWwindow* window = createWindow(windowWidth, windowHeight, "W9-W11");

    if (!window) {
        return -1;
    }

    glm::mat4 projection = glm::perspective(
        glm::radians(70.0f),                            // fov in y dir.
        (float)windowWidth / (float)windowHeight,       //fov in x dir.
        0.1f,
        100.0f
    );

    FreeCamera camera;              // creates the camera with the free_camera.cpp file functions
    LightController lightController;    //create this to manually control the light with I/J/K/L

    //shaders
    GLuint shaderProgram = createShaderProgram(
        "src/week09/vertex.glsl",
        "src/week09/fragment.glsl"
    );

    GLuint lampShaderProgram = createShaderProgram(
        "src/week09/lamp_vertex.glsl",
        "src/week09/lamp_fragment.glsl"
    );

    GLuint checkerboard = loadTexture("assets/textures/checkerboard.png");

    CubeMesh cube = createCubeMesh();    
    SphereMesh sphere = createSphereMesh();

    Material dullOrange = createDullOrangeMaterial();
    Material shinyBlue = createShinyBlueMaterial();
    Material matteGray = createMatteGrayMaterial();

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLuint secondLightPosLoc = glGetUniformLocation(shaderProgram, "secondLightPos");
    GLuint secondLightColorLoc = glGetUniformLocation(shaderProgram, "secondLightColor");

    GLuint lampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
    GLuint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
    GLuint lampProjectionLoc = glGetUniformLocation(lampShaderProgram, "projection");
    GLuint lampColorLoc = glGetUniformLocation(lampShaderProgram, "lampColor");

    GLuint light1EnabledLoc = glGetUniformLocation(shaderProgram, "light1Enabled");
    GLuint light2EnabledLoc = glGetUniformLocation(shaderProgram, "light2Enabled");

    bool light1Enabled = true;
    bool light2Enabled = true;

    bool key1WasPressed = false;
    bool key2WasPressed = false;

    while (!glfwWindowShouldClose(window))
    {
        camera.update(window);
        lightController.update(window);

        bool key1IsPressed = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
        bool key2IsPressed = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;

        if (key1IsPressed && !key1WasPressed)
        {
            light1Enabled = !light1Enabled;
        }

        if (key2IsPressed && !key2WasPressed)
        {
            light2Enabled = !light2Enabled;
        }

        key1WasPressed = key1IsPressed;
        key2WasPressed = key2IsPressed;

        glm::mat4 view = camera.getViewMatrix();

        glm::vec3 lightPos = lightController.getPosition();
        glm::vec3 lightColor(1.25f, 1.25f, 1.25f);

        glm::vec3 secondLightPos(-2.0f, 2.2f, -2.0f);
        glm::vec3 secondLightColor(0.3f, 0.5f, 1.0f);

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.15f));

        glm::mat4 secondLightModel = glm::mat4(1.0f);
        secondLightModel = glm::translate(secondLightModel, secondLightPos);
        secondLightModel = glm::scale(secondLightModel, glm::vec3(0.15f));

        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw lit objects
        glUseProgram(shaderProgram);

        glUniform1i(light1EnabledLoc, light1Enabled);
        glUniform1i(light2EnabledLoc, light2Enabled);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.getPosition()));

        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        glUniform3fv(secondLightPosLoc, 1, glm::value_ptr(secondLightPos));
        glUniform3fv(secondLightColorLoc, 1, glm::value_ptr(secondLightColor));

        // test cube
        glm::mat4 testCube = glm::mat4(1.0f);
        testCube = glm::translate(testCube, glm::vec3(-0.2f, -0.6f, 0.0f));

        setMaterial(shaderProgram, dullOrange);
        drawTexturedCube(cube.VAO, modelLoc, testCube, checkerboard);

        // test sphere
        glm::mat4 testSphere = glm::mat4(1.0f);
        testSphere = glm::translate(testSphere, glm::vec3(0.8f, -0.4f, 0.0f));

        setMaterial(shaderProgram, shinyBlue);
        drawSphere(sphere, modelLoc, testSphere);

        // room
        setMaterial(shaderProgram, matteGray);
        drawClosedRoom(cube.VAO, modelLoc, checkerboard);


        // Draw lamp marker
        glUseProgram(lampShaderProgram);

        glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(lampProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // first movable lamp
        if (light1Enabled)
        {
            glUniform3f(lampColorLoc, 1.0f, 1.0f, 0.3f);
            drawCube(cube.VAO, lampModelLoc, lightModel);
        }

        // second fixed lamp
        if (light2Enabled)
        {
            glUniform3f(lampColorLoc, 0.3f, 0.5f, 1.0f);
            drawCube(cube.VAO, lampModelLoc, secondLightModel);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    deleteCubeMesh(cube);
    deleteSphereMesh(sphere);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(lampShaderProgram);

    glfwTerminate();
    return 0;
}