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

int main()
{
    // GLFW init
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "W9", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);        // enable the mouse rotation to go over 360 degrees

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD loading failed\n";
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);                    // enable depth in order to only see the closest face of an object
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::mat4 projection = glm::perspective(  
        glm::radians(70.0f),                    // fov in y direction
        800.0f / 600.0f,                        // fov in x direction
        0.1f,                                   // nearest visible spot
        100.0f                                  // farthest visible spot
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

    // textures
    GLuint checkerboard = loadTexture("assets/textures/checkerboard.png");

    CubeMesh cube = createCubeMesh();    
    SphereMesh sphere = createSphereMesh();

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

    GLuint lampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
    GLuint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
    GLuint lampProjectionLoc = glGetUniformLocation(lampShaderProgram, "projection");

    while (!glfwWindowShouldClose(window))
    {
        camera.update(window);
        lightController.update(window);

        glm::mat4 view = camera.getViewMatrix();

        glm::vec3 lightPos = lightController.getPosition();
        glm::vec3 lightColor(1.25f, 1.25f, 1.25f);

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.15f));

        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw lit objects
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.getPosition()));
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        // test cube
        glm::mat4 testCube = glm::mat4(1.0f);
        testCube = glm::translate(testCube, glm::vec3(-0.2f, 0.0f, 0.0f));

        glUniform3f(objectColorLoc, 0.9f, 0.4f, 0.2f);
        drawTexturedCube(cube.VAO, modelLoc, testCube, checkerboard);

        // test sphere
        glm::mat4 testSphere = glm::mat4(1.0f);
        testSphere = glm::translate(testSphere, glm::vec3(0.8f, 0.0f, 0.0f));

        glUniform3f(objectColorLoc, 0.2f, 0.5f, 1.0f);
        drawSphere(sphere, modelLoc, testSphere);

        // room
        glUniform3f(objectColorLoc, 0.7f, 0.7f, 0.7f);
        drawClosedRoom(cube.VAO, modelLoc, checkerboard);


        // Draw lamp marker
        glUseProgram(lampShaderProgram);

        glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(lampProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        drawCube(cube.VAO, lampModelLoc, lightModel);

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