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
#include "../common/free_camera.h"
#include "../common/texture.h"
#include "../common/character_skin.h"
#include "../common/cubemap_texture.h"
#include "../common/skybox.h"

//shader files

std::string readShaderFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static void drawGround(GLuint VAO, GLuint modelLoc, float t, GLuint texture) {
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
        drawTexturedCube(VAO, modelLoc, tile, texture);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "W5-W6", nullptr, nullptr);
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

    FreeCamera camera;              // creates the camera with the free_camera.cpp file functions
    SkyBox skybox(&camera);         //creates skybox

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

    // textures
    GLuint checkerboard = loadTexture("assets/textures/checkerboard.png");
    GLuint ground = loadTexture("assets/textures/ground.jpg");
    CharacterSkin skin1 = loadCharacterSkin("assets/textures/squirrel_boy.png");
    CharacterSkin skin2 = loadCharacterSkin("assets/textures/squirrel_girl.png");

    CubeMesh cube = createCubeMesh();           
    CubeMesh characterCube = createCubeMesh();

    //sky box
    if (!skybox.Init(
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

    //perspective view
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");         // return the location of the model in the shader memory
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    while (!glfwWindowShouldClose(window))
    {
        camera.update(window);

        glm::mat4 view = camera.getViewMatrix();

        float t = glfwGetTime();

        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // decide which surface is the closest through the depth buffer

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::vec3 lightPos(0.0f, 2.0f, 2.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.15f));
        
        glm::mat4 character1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        glm::mat4 character2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        drawCharacter(characterCube, modelLoc, t, character1, skin1);
        drawCharacter(characterCube, modelLoc, t + 1.0f, character2, skin2);

        drawTexturedCube(cube.VAO, modelLoc, lightModel, checkerboard);     // draw the light source, looks a bit buggy for now

        //test for cube texture
        glm::mat4 testCube = glm::mat4(1.0f);
        testCube = glm::translate(testCube, glm::vec3(0.0f, 0.0f, 0.0f));
        drawTexturedCube(cube.VAO, modelLoc, testCube, checkerboard);

        drawGround(cube.VAO, modelLoc, t, ground);

        skybox.Render(projection);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    deleteCubeMesh(cube);
    deleteCubeMesh(characterCube);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}