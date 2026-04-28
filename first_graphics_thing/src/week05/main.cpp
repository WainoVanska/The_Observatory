#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../common/character.h"
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

    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);          // the place in 3D space where the camera is located 
    float cameraSpeed = 0.01f;
    double lastMousePosX = 0.0f;
    double lastMousePosY = 0.0f;
    float cameraMouseX = 0.0f;
    float cameraMouseY = 0.0f;

    glm::vec3 cameraFront;          // the direction the camera is looking

    float yaw = -90.0f;             // turning left / right ("FPS POV" around the Y-axis)
    float pitch = 0.0f;             // looking up / down ( around the X-axis)
    float sensitivity = 0.05f;       // sensitivity of the mouse

    while (!glfwWindowShouldClose(window))
    {

        offset += 0.002f;

        float t = glfwGetTime();

        float characterZ = -t * 0.5f;

        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));       // gives perpendicular vector to the camera (vec to the right)

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraFront;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraFront;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraRight;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraRight;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            cameraPos += cameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            cameraPos -= cameraSpeed;
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

        yaw += mouseOffsetX * sensitivity;
        pitch -= mouseOffsetY * sensitivity;

        //converts the mouse's x and y values to angles --> then make them direction vectors
        cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
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