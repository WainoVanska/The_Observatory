#include "sphere.h"

#include <vector>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

/*
with current counts:
vertices: 561
triangles: 960
indices: 2880
*/

SphereMesh createSphereMesh(float radius, unsigned int sectorCount, unsigned int stackCount)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;

    for (unsigned int i = 0; i <= stackCount; ++i) // stack count = how many slices around the sphere (latitude)
    {
        float stackAngle = PI / 2.0f - i * PI / stackCount;
        float xy = radius * cosf(stackAngle); // radius of horizontal circle at z height
        float z = radius * sinf(stackAngle);  // height of ring

        for (unsigned int j = 0; j <= sectorCount; ++j) // how many horizontal layers from top to bottom (longitude)
        {
            float sectorAngle = j * 2.0f * PI / sectorCount;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

            float u = (float)j / sectorCount;
            float v = (float)i / stackCount;

            // position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // color
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);

            // UV
            vertices.push_back(u);
            vertices.push_back(v);

            // normal
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    for (unsigned int i = 0; i < stackCount; ++i)
    {
        unsigned int k1 = i * (sectorCount + 1); // ring
        unsigned int k2 = k1 + sectorCount + 1;  // ring below it

        for (unsigned int j = 0; j < sectorCount;
             ++j, ++k1, ++k2) // loop connects rectangle shaped area between the rings together with two triangles
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != stackCount - 1)
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    SphereMesh sphere{};
    sphere.indexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &sphere.VAO);
    glGenBuffers(1, &sphere.VBO);
    glGenBuffers(1, &sphere.EBO);

    glBindVertexArray(sphere.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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

    return sphere;
}

void deleteSphereMesh(const SphereMesh& sphere)
{
    glDeleteVertexArrays(1, &sphere.VAO);
    glDeleteBuffers(1, &sphere.VBO);
    glDeleteBuffers(1, &sphere.EBO);
}

void drawSphere(const SphereMesh& sphere, GLuint modelLoc, const glm::mat4& model)
{
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(sphere.VAO);
    glDrawElements(GL_TRIANGLES, sphere.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
