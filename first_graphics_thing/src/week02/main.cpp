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


    float vertices[] = {
        // positions           // colors
        -0.25f, -0.25f, 0.0f,  1.0f, 0.3f, 1.0f,
         0.25f, -0.25f, 0.0f,  0.5f, 1.0f, 1.0f,
         0.25f,  0.25f, 0.0f,  1.0f, 1.0f, 0.4f,
        -0.25f,  0.25f, 0.0f,  0.8f, 1.0f, 0.9f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
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

    GLfloat offset = 0.0f;



    while (!glfwWindowShouldClose(window))
    {

        offset += 0.2f;
        //vector init

        glm::vec2 res(0.3f, 0.3f);


        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        //identity matrix creation
        glm::mat4 transform = glm::mat4(1.0f);

        //CONTINUE FROM EXERCISE: B2


        //translation before scaling vs. after test
        //transform = glm::translate(transform, glm::vec3(0.3f, 0.0f, 0.0f));                              //translates to a given point
        //transform = glm::scale(transform, glm::vec3(2.0f, 2.0f, 2.0f));                                  //scaling


        //basics
        //transform = glm::translate(transform, glm::vec3(0.1f, 0.2f, 0.0f));                              //translates to a given point
        //transform = glm::rotate(transform, glm::radians(offset), glm::vec3(0.0f, 0.0f, 1.0f));           //rotates around the z-axis
        //transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));                                  //scaling



        // communicate with vertex shader, get the mat4 transform matrix shader
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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