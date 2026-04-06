#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// source code saved as chars for GLSL language for vertex shader
const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform vec2 offset;\n"        //offset vec
"uniform vec3 offsetcolor;\n"   //color offset
"out vec3 ourColor;\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(aPos + offset, 0.0, 1.0);\n"
"    ourColor = clamp(aColor + offsetcolor, 0.0, 1.0);\n"
"}\n";

// source code saved as chars for GLSL language for fragment shader

const char* fragmentShaderSource =
"#version 330 core\n"
"in vec3 ourColor;\n"

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"    FragColor = vec4(ourColor, 1.0);\n"
"}\n";

int main()
{
    //initializing the glfw
    if (!glfwInit())
    {
        std::cout << "GLFW init failed\n";
        return -1;
    }
    // establish what version of glfw we're using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //creating a new glfw window with glfw function
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL + GLAD Test", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    // we establish the context
    glfwMakeContextCurrent(window);

    // I dont understand
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD loading failed\n";
        glfwTerminate();
        return -1;
    }

    // loading the window
    glViewport(0, 0, 800, 600);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);      //using this for wireframe mode _LINE instead of _FILL if needed

    //data for vertices
    GLfloat vertices[] = {
        -1.0f, -1.0f,       1.0f, 0.3f, 1.0f,
        -1.0f,  0.0f,       1.0f, 1.0f, 1.0f,
         0.0f, -1.0f,       1.0f, 1.0f, 1.0f,
         0.0f,  0.0f,       1.0f, 1.0f, 1.0f
    };

    //indices for vertices
    unsigned int indices[] = {
        0,1,2,      //triangle 1
        2,1,3       //triangle 2
    };

    // vertex array object init
	unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex buffer object init
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //element buffer object init
	unsigned int EBO; 
	glGenBuffers(1, &EBO); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);       // for pos
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));       // for rgb

	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);



    //vertex shader creation
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);   
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);   
	glCompileShader(vertexShader);   

    //fragment shader creation
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    //attaching the two to the shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //deleting the vertex shaders and fragment shaders since we don't need them anymore separately
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLfloat xOffset = 0.5f;
    GLfloat yOffset = 1.3f;
    GLfloat colorOffset = 0.0f;
    GLfloat gravity = 0.00981f;
    int index = 1;
    bool bounced = false;
    bool backwards = false;
    bool changecolorback = false;

    //render loop?
    while (!glfwWindowShouldClose(window))
    {
        GLfloat speed = (gravity / 60) * index;

        if (!bounced) {
            yOffset -= speed;
            index += 1;
        }

        if (bounced) {
            yOffset += speed;
            index -= 1;
            if (index <= 0) {
                bounced = false;
                index = 1;
            }
        }
        
        if (yOffset < 0.0f) {
            yOffset = 0.0f;
            bounced = true;
            index -= 1;
        }

        if (!backwards) {
            xOffset += 0.002f;
            if (xOffset >= 1.0f) {
                backwards = true;
            }

        }
        if (backwards) {
            xOffset -= 0.002f;
            if (xOffset < 0.0f) {
                backwards = false;
            }
        }

        if (!changecolorback) {
            colorOffset += 0.001f;
            if (colorOffset >= 1.0f) {
                colorOffset = 1.0f;
                changecolorback = true;
            }
        }
        else {
            colorOffset -= 0.001f;
            if (colorOffset <= 0.0f) {
                colorOffset = 0.0f;
                changecolorback = false;
            }
        }
            

        glClearColor(1.0f, 0.7f, 1.0f, 1.0f);       //color of the background
        glClear(GL_COLOR_BUFFER_BIT);               
        glUseProgram(shaderProgram);                //using the shader program

        GLint offsetLocation = glGetUniformLocation(shaderProgram, "offset");             //getting the offset location
        glUniform2f(offsetLocation, xOffset, yOffset);                // change the position by this offset

        GLint offsetColor = glGetUniformLocation(shaderProgram, "offsetcolor");

        glUniform3f(offsetColor, colorOffset, -colorOffset, 0.0f);
		glBindVertexArray(VAO);                     // ??
		glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT, 0);           //draws triangles with the data from elements
        
        //buffer swap to get the live feed, and polling the events
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }
    //termination

    glfwTerminate();
    return 0;
}