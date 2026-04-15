#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//shader files

std::string readShaderFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


float offset = 0.0f;




//scalable cube creation function so you easily make more than one if needed
void drawCube(GLuint VAO, GLuint transformLoc, const glm::mat4& transform)
{
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "W2/W3", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD loading failed\n";
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);                    // enable depth in order to only see the closest face of an object
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float vertices[] = {
        // positions           // colors
        -0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,    //front side
         0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,
         0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,
        -0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,

         0.25f, -0.25f,-0.25f,  0.7f, 0.7f, 0.3f,    //right side
         0.25f, -0.25f, 0.25f,  0.7f, 0.7f, 0.3f,
         0.25f,  0.25f, 0.25f,  0.7f, 0.7f, 0.3f,
         0.25f,  0.25f,-0.25f,  0.7f, 0.7f, 0.3f,

         0.25f, -0.25f, 0.25f,  0.0f, 0.5f, 0.5f,    //back side
        -0.25f, -0.25f, 0.25f,  0.0f, 0.5f, 0.5f,
        -0.25f,  0.25f, 0.25f,  0.0f, 0.5f, 0.5f,
         0.25f,  0.25f, 0.25f,  0.0f, 0.5f, 0.5f,

        -0.25f, -0.25f, 0.25f,  0.3f, 0.6f, 0.3f,    //left side
        -0.25f, -0.25f,-0.25f,  0.3f, 0.6f, 0.3f,
        -0.25f,  0.25f,-0.25f,  0.3f, 0.6f, 0.3f,
        -0.25f,  0.25f, 0.25f,  0.3f, 0.6f, 0.3f,

        -0.25f,  0.25f,-0.25f,  0.1f, 0.3f, 1.0f,    //top side
         0.25f,  0.25f,-0.25f,  0.1f, 0.3f, 1.0f,
         0.25f,  0.25f, 0.25f,  0.1f, 0.3f, 1.0f,
        -0.25f,  0.25f, 0.25f,  0.1f, 0.3f, 1.0f,

        -0.25f, -0.25f,-0.25f,  1.0f, 0.3f, 1.0f,    //bottom side
         0.25f, -0.25f,-0.25f,  1.0f, 0.3f, 1.0f,
         0.25f, -0.25f, 0.25f,  1.0f, 0.3f, 1.0f,
        -0.25f, -0.25f, 0.25f,  1.0f, 0.3f, 1.0f
    };

    unsigned int indices[] = {
        0, 2, 1,   2, 0, 3,      // front
        4, 6, 5,   6, 4, 7,      // right
        8, 10,9,   10,8, 11,      // back
        12,14,13,  14,12,15,      // left
        16,18,17,  18,16,19,      // top
        20,22,21,  22,20,23       // bottom
    };


    // VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //shaders
    std::string vertexShaderCode = readShaderFile("src/week02/vertex.glsl");
    std::string fragmentShaderCode = readShaderFile("src/week02/fragment.glsl");

    const char* vertexShaderSource = vertexShaderCode.c_str();              //??
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

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");

    while (!glfwWindowShouldClose(window))
    {
        float legAngle = sin(glfwGetTime() * 5) * 30.0f;
        float armAngle = sin(glfwGetTime() * 5) * 60.0f;
        float headAngle = sin(glfwGetTime() * 10) * 2.0f;
        float bodyMovement = sin(glfwGetTime() * 10) * 2.0f;
        float cameraAngle = glfwGetTime() * 50.0f;

        offset += 0.002f;

        if (offset > 1.8) {
            offset = -1.68f;

        }



        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // decide which surface is the closest through the depth buffer

        glUseProgram(shaderProgram);
 
        // character object creation so that child objects are then mounted to this
        glm::mat4 character = glm::mat4(1.0f);
        character = glm::translate(character, glm::vec3(-offset, bodyMovement / 100, 0.0f));
        character = glm::rotate(character, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //character = glm::rotate(character, glm::radians(cameraAngle), glm::vec3(0.0f, 1.0f, 0.0f));




        // body
        glm::mat4 body = character;
        body = glm::scale(body, glm::vec3(0.9f, 1.5f, 0.4f));
        drawCube(VAO, transformLoc, body);


        // head
        glm::mat4 head = character;
        head = glm::translate(head, glm::vec3(0.0f, (headAngle / 100) + 0.55f, 0.0f));
        head = glm::scale(head, glm::vec3(0.6f, 0.6f, 0.6f));
        drawCube(VAO, transformLoc, head);


        // left leg
        glm::mat4 leftLeg = character;
        leftLeg = glm::translate(leftLeg, glm::vec3(-0.20f, -0.22f, 0.0f)); 
        leftLeg = glm::rotate(leftLeg, glm::radians(legAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        leftLeg = glm::translate(leftLeg, glm::vec3(0.0f, -0.5f, 0.0f));
        leftLeg = glm::scale(leftLeg, glm::vec3(0.2f, 1.5f, 0.2f));
        drawCube(VAO, transformLoc, leftLeg);


        // right leg
        glm::mat4 rightLeg = character;
        rightLeg = glm::translate(rightLeg, glm::vec3(0.20f, -0.22f, 0.0f));
        rightLeg = glm::rotate(rightLeg, glm::radians(-legAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        rightLeg = glm::translate(rightLeg, glm::vec3(0.0f, -0.5f, 0.0f));
        rightLeg = glm::scale(rightLeg, glm::vec3(0.2f, 1.5f, 0.2f));
        drawCube(VAO, transformLoc, rightLeg);


        // left arm
        glm::mat4 leftArm = character;
        leftArm = glm::translate(leftArm, glm::vec3(-0.35f, 0.20f, 0.1f)); 
        leftArm = glm::rotate(leftArm, glm::radians(-armAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        leftArm = glm::translate(leftArm, glm::vec3(0.0f, -0.4f, 0.0f));
        leftArm = glm::scale(leftArm, glm::vec3(0.2f, 1.7f, 0.2f));
        drawCube(VAO, transformLoc, leftArm);


        // right arm
        glm::mat4 rightArm = character;
        rightArm = glm::translate(rightArm, glm::vec3(0.35f, 0.20f, -0.1f));
        rightArm = glm::rotate(rightArm, glm::radians(armAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        rightArm = glm::translate(rightArm, glm::vec3(0.0f, -0.4f, 0.0f));
        rightArm = glm::scale(rightArm, glm::vec3(0.2f, 1.7f, 0.2f));
        drawCube(VAO, transformLoc, rightArm);

        /* 
           ||||
            ||
        ----||----
            ||
           |  |
        */
        

        glfwSwapBuffers(window);
        glfwPollEvents();

        //pressing esc to quit simulation
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}