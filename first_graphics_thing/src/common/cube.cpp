#include "cube.h"
#include <glm/gtc/type_ptr.hpp>

CubeMesh createCubeMesh()
{
    float vertices[] = {
        // positions            // colors           // UVs 
        -0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,    //front side
         0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 
         0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

         0.25f, -0.25f,-0.25f,  0.7f, 0.7f, 0.3f,   0.0f, 0.0f,    //right side
         0.25f, -0.25f, 0.25f,  0.7f, 0.7f, 0.3f,   1.0f, 0.0f,
         0.25f,  0.25f, 0.25f,  0.7f, 0.7f, 0.3f,   1.0f, 1.0f,
         0.25f,  0.25f,-0.25f,  0.7f, 0.7f, 0.3f,   0.0f, 1.0f,

         0.25f, -0.25f, 0.25f,  0.0f, 0.5f, 0.5f,   0.0f, 0.0f,    //back side
        -0.25f, -0.25f, 0.25f,  0.0f, 0.5f, 0.5f,   1.0f, 0.0f,
        -0.25f,  0.25f, 0.25f,  0.0f, 0.5f, 0.5f,   1.0f, 1.0f,
         0.25f,  0.25f, 0.25f,  0.0f, 0.5f, 0.5f,   0.0f, 1.0f, 

        -0.25f, -0.25f, 0.25f,  0.3f, 0.6f, 0.3f,   0.0f, 0.0f,    //left side
        -0.25f, -0.25f,-0.25f,  0.3f, 0.6f, 0.3f,   1.0f, 0.0f,
        -0.25f,  0.25f,-0.25f,  0.3f, 0.6f, 0.3f,   1.0f, 1.0f,
        -0.25f,  0.25f, 0.25f,  0.3f, 0.6f, 0.3f,   0.0f, 1.0f,

        -0.25f,  0.25f,-0.25f,  0.1f, 0.3f, 1.0f,   0.0f, 0.0f,    //top side
         0.25f,  0.25f,-0.25f,  0.1f, 0.3f, 1.0f,   1.0f, 0.0f,
         0.25f,  0.25f, 0.25f,  0.1f, 0.3f, 1.0f,   1.0f, 1.0f,
        -0.25f,  0.25f, 0.25f,  0.1f, 0.3f, 1.0f,   0.0f, 1.0f,

        -0.25f, -0.25f,-0.25f,  1.0f, 0.3f, 1.0f,   0.0f, 0.0f,    //bottom side
         0.25f, -0.25f,-0.25f,  1.0f, 0.3f, 1.0f,   1.0f, 0.0f,
         0.25f, -0.25f, 0.25f,  1.0f, 0.3f, 1.0f,   1.0f, 1.0f,
        -0.25f, -0.25f, 0.25f,  1.0f, 0.3f, 1.0f,   0.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 2, 1,   2, 0, 3,      // front
        4, 6, 5,   6, 4, 7,      // right
        8, 10,9,   10,8, 11,      // back
        12,14,13,  14,12,15,      // left
        16,18,17,  18,16,19,      // top
        20,22,21,  22,20,23       // bottom
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

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

static void setFaceUV(float* vertices, int faceIndex, const FaceUV& uv)
{
    int startVertex = faceIndex * 4;

    // Each vertex has 8 floats:
    // position 3 + color 3 + uv 2
    int stride = 8;

    float faceUVs[4][2] = {
        { uv.u1, uv.v1 },
        { uv.u2, uv.v1 },
        { uv.u2, uv.v2 },
        { uv.u1, uv.v2 }
    };

    for (int i = 0; i < 4; i++)
    {
        int vertexIndex = startVertex + i;
        vertices[vertexIndex * stride + 6] = faceUVs[i][0];
        vertices[vertexIndex * stride + 7] = faceUVs[i][1];
    }
}

void drawCubeWithUVs(
    const CubeMesh& cube,
    GLuint modelLoc,
    const glm::mat4& model,
    const BoxUVs& uvs
)
{
    float vertices[] = {
        // positions            // colors           // UVs 
        -0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
         0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

         0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
         0.25f, -0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.25f,  0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

         0.25f, -0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.25f, -0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.25f,  0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         0.25f,  0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        -0.25f, -0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.25f,  0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        -0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
         0.25f,  0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.25f,  0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.25f,  0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        -0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
         0.25f, -0.25f,-0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.25f, -0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.25f, -0.25f, 0.25f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    };

    setFaceUV(vertices, 0, uvs.front);
    setFaceUV(vertices, 1, uvs.right);
    setFaceUV(vertices, 2, uvs.back);
    setFaceUV(vertices, 3, uvs.left);
    setFaceUV(vertices, 4, uvs.top);
    setFaceUV(vertices, 5, uvs.bottom);

    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    drawCube(cube.VAO, modelLoc, model);
}

void drawSkyboxCube(const CubeMesh& cube)
{
    glBindVertexArray(cube.VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}