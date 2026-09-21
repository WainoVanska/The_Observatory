#include "cube.h"
#include <glm/gtc/type_ptr.hpp>

CubeMesh createCubeMesh()
{
    float vertices[] = {
        // positions            // colors           // UVs        // normal vectors
        -0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,    //front side
         0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
         0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,
        -0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,

         0.25f, -0.25f,-0.25f,  0.7f, 0.7f, 0.3f,   0.0f, 0.0f,   1.0f, 0.0f,  0.0f,    //right side
         0.25f, -0.25f, 0.25f,  0.7f, 0.7f, 0.3f,   1.0f, 0.0f,   1.0f, 0.0f,  0.0f,
         0.25f,  0.25f, 0.25f,  0.7f, 0.7f, 0.3f,   1.0f, 1.0f,   1.0f, 0.0f,  0.0f,
         0.25f,  0.25f,-0.25f,  0.7f, 0.7f, 0.3f,   0.0f, 1.0f,   1.0f, 0.0f,  0.0f,

         0.25f, -0.25f, 0.25f,  0.0f, 0.5f, 0.5f,   0.0f, 0.0f,   0.0f, 0.0f,  1.0f,    //back side
        -0.25f, -0.25f, 0.25f,  0.0f, 0.5f, 0.5f,   1.0f, 0.0f,   0.0f, 0.0f,  1.0f,
        -0.25f,  0.25f, 0.25f,  0.0f, 0.5f, 0.5f,   1.0f, 1.0f,   0.0f, 0.0f,  1.0f,
         0.25f,  0.25f, 0.25f,  0.0f, 0.5f, 0.5f,   0.0f, 1.0f,   0.0f, 0.0f,  1.0f,

        -0.25f, -0.25f, 0.25f,  0.3f, 0.6f, 0.3f,   0.0f, 0.0f,  -1.0f, 0.0f,  0.0f,    //left side
        -0.25f, -0.25f,-0.25f,  0.3f, 0.6f, 0.3f,   1.0f, 0.0f,  -1.0f, 0.0f,  0.0f,
        -0.25f,  0.25f,-0.25f,  0.3f, 0.6f, 0.3f,   1.0f, 1.0f,  -1.0f, 0.0f,  0.0f,
        -0.25f,  0.25f, 0.25f,  0.3f, 0.6f, 0.3f,   0.0f, 1.0f,  -1.0f, 0.0f,  0.0f,

        -0.25f,  0.25f,-0.25f,  0.1f, 0.3f, 1.0f,   0.0f, 0.0f,   0.0f, 1.0f,  0.0f,    //top side
         0.25f,  0.25f,-0.25f,  0.1f, 0.3f, 1.0f,   1.0f, 0.0f,   0.0f, 1.0f,  0.0f,
         0.25f,  0.25f, 0.25f,  0.1f, 0.3f, 1.0f,   1.0f, 1.0f,   0.0f, 1.0f,  0.0f,
        -0.25f,  0.25f, 0.25f,  0.1f, 0.3f, 1.0f,   0.0f, 1.0f,   0.0f, 1.0f,  0.0f,

        -0.25f, -0.25f,-0.25f,  1.0f, 0.3f, 1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,    //bottom side
         0.25f, -0.25f,-0.25f,  1.0f, 0.3f, 1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
         0.25f, -0.25f, 0.25f,  1.0f, 0.3f, 1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
        -0.25f, -0.25f, 0.25f,  1.0f, 0.3f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 2, 1,   2, 0, 3,      // front
        4, 6, 5,   6, 4, 7,      // right
        8, 10,9,   10,8, 11,      // back
        12,14,13,  14,12,15,      // left
        16,18,17,  18,16,19,      // top
        20,21,22,  22,23,20       // bottom
    };

    CubeMesh cube{};

    glGenVertexArrays(1, &cube.VAO);
    glGenBuffers(1, &cube.VBO);
    glGenBuffers(1, &cube.EBO);

    glBindVertexArray(cube.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    return cube;
}

void deleteCubeMesh(const CubeMesh& cube)
{
    glDeleteVertexArrays(1, &cube.VAO);
    glDeleteBuffers(1, &cube.VBO);
    glDeleteBuffers(1, &cube.EBO);
}

void drawCube(GLuint VAO, GLuint modelLoc, const glm::mat4& model)
{
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void drawTexturedCube(GLuint VAO, GLuint modelLoc, const glm::mat4& model, GLuint texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    drawCube(VAO, modelLoc, model);
}
void drawTexturedCubeWithSpecularMap(
    GLuint VAO,
    GLuint modelLoc,
    const glm::mat4& model,
    GLuint diffuseTexture,
    GLuint specularTexture
)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularTexture);

    drawCube(VAO, modelLoc, model);
}
