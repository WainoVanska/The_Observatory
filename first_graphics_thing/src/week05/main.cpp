#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../common/cube.h"

//shader files

std::string readShaderFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

float offset = 0.0f;

static void drawCube(GLuint VAO, GLuint modelLoc, const glm::mat4& model)
{
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

static void drawCharacter(GLuint VAO, GLuint modelLoc, float t, const glm::mat4& baseTransform)
{
    float legAngle = sin(t * 5.0f) * 30.0f;
    float armAngle = sin(t * 5.0f) * 60.0f;
    float headAngle = sin(t * 10.0f) * 2.0f;
    float bodyMovement = sin(t * 10.0f) * 2.0f;

    glm::mat4 character = baseTransform;

    character = glm::translate(character, glm::vec3(0.0f, bodyMovement / 100.0f, 0.0f));
    character = glm::rotate(character, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 body = character;
    body = glm::scale(body, glm::vec3(0.9f, 1.5f, 0.4f));
    drawCube(VAO, modelLoc, body);

    glm::mat4 head = character;
    head = glm::translate(head, glm::vec3(0.0f, (headAngle / 100.0f) + 0.55f, 0.0f));
    head = glm::scale(head, glm::vec3(0.6f, 0.6f, 0.6f));
    drawCube(VAO, modelLoc, head);

    glm::mat4 leftLeg = character;
    leftLeg = glm::translate(leftLeg, glm::vec3(-0.20f, -0.22f, 0.0f));
    leftLeg = glm::rotate(leftLeg, glm::radians(legAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    leftLeg = glm::translate(leftLeg, glm::vec3(0.0f, -0.5f, 0.0f));
    leftLeg = glm::scale(leftLeg, glm::vec3(0.2f, 1.5f, 0.2f));
    drawCube(VAO, modelLoc, leftLeg);

    glm::mat4 rightLeg = character;
    rightLeg = glm::translate(rightLeg, glm::vec3(0.20f, -0.22f, 0.0f));
    rightLeg = glm::rotate(rightLeg, glm::radians(-legAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    rightLeg = glm::translate(rightLeg, glm::vec3(0.0f, -0.5f, 0.0f));
    rightLeg = glm::scale(rightLeg, glm::vec3(0.2f, 1.5f, 0.2f));
    drawCube(VAO, modelLoc, rightLeg);

    glm::mat4 leftArm = character;
    leftArm = glm::translate(leftArm, glm::vec3(-0.35f, 0.20f, 0.1f));
    leftArm = glm::rotate(leftArm, glm::radians(-armAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    leftArm = glm::translate(leftArm, glm::vec3(0.0f, -0.4f, 0.0f));
    leftArm = glm::scale(leftArm, glm::vec3(0.2f, 1.7f, 0.2f));
    drawCube(VAO, modelLoc, leftArm);

    glm::mat4 rightArm = character;
    rightArm = glm::translate(rightArm, glm::vec3(0.35f, 0.20f, -0.1f));
    rightArm = glm::rotate(rightArm, glm::radians(armAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    rightArm = glm::translate(rightArm, glm::vec3(0.0f, -0.4f, 0.0f));
    rightArm = glm::scale(rightArm, glm::vec3(0.2f, 1.7f, 0.2f));
    drawCube(VAO, modelLoc, rightArm);
}

static void drawGround(GLuint VAO, GLuint modelLoc, float t) {
    float walkSpeed = 1.5f;
    float groundX = -t * walkSpeed;

    float spacing = 2.0f;
    float loopLength = 20.0f;

    for (int i = 0; i < 10; i++)
    {
        float tileX = groundX + i * spacing;

        while (tileX < -10.0f)
            tileX += loopLength;

        glm::mat4 tile = glm::mat4(1.0f);
        tile = glm::translate(tile, glm::vec3(tileX, -1.2f, 0.0f));
        tile = glm::scale(tile, glm::vec3(2.0f, 0.08f, 1.2f));
        drawCube(VAO, modelLoc, tile);
    }
}

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "W5", nullptr, nullptr);
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
        glm::radians(45.0f),                    // fov in y direction
        800.0f / 600.0f,                        // fov in x direction
        0.1f,                                   // nearest visible spot
        100.0f                                  // farthest visible spot
    );

    //shaders
    std::string vertexShaderCode = readShaderFile("src/week05/vertex.glsl");
    std::string fragmentShaderCode = readShaderFile("src/week05/fragment.glsl");

    const char* vertexShaderSource = vertexShaderCode.c_str();
    const char* fragmentShaderSource = fragmentShaderCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");         // return the location of the model in the shader memory
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    CubeMesh cube = createCubeMesh();           // create the cube through externalized function

    float w = 0.0f;
    float s = 0.0f;
    float up = 0.0f;
    double lastMousePosX = 0.0f;
    double lastMousePosY = 0.0f;
    float cameraMouseX = 0.0f;
    float cameraMouseY = 0.0f;

    glm::vec3 cameraFront;

    float yaw = -90.0f;             // turning left / right
    float pitch = 0.0f;             // looking up / down
    float sensitivity = 0.2f;       // sensitivity of the mouse

    while (!glfwWindowShouldClose(window))
    {

        offset += 0.002f;

        float t = glfwGetTime();

        float characterZ = -t * 0.5f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            w -= 0.01f;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            w += 0.01f;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            s += 0.01f;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            s -= 0.01f;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            up += 0.01f;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            up -= 0.01f;
        }

        double mousePosX;
        double mousePosY;

        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        
        float mouseOffsetX = mousePosX - lastMousePosX;
        float mouseOffsetY = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        cameraMouseX += mouseOffsetX * 0.02f;
        cameraMouseY += mouseOffsetY * 0.02f;

        glm::vec3 cameraPos = glm::vec3(s, 1.0f + up, 3.0f + w);

        yaw += mouseOffsetX * sensitivity;
        pitch -= mouseOffsetY * sensitivity;

        glm::vec3 cameraFront;

        cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));          // ???
        cameraFront.y = sin(glm::radians(pitch));
        cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFront = glm::normalize(cameraFront);          // normalize back to [0,1] range

        glm::mat4 view = glm::lookAt(
            cameraPos,                              // camera position, where it's placed
            cameraPos + cameraFront,                // look at character, what direction the camera is in
            glm::vec3(0.0f, 1.0f, 0.0f)             // orientation, basically rotation of the camera itself
        );

        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // decide which surface is the closest through the depth buffer

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glm::mat4 character1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        glm::mat4 character2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        drawCharacter(cube.VAO, modelLoc, t, character1);
        drawCharacter(cube.VAO, modelLoc, t + 1.0f, character2);

        drawGround(cube.VAO, modelLoc, t);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    deleteCubeMesh(cube);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}