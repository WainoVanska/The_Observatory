#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct SphereMesh
{
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    unsigned int indexCount;
};

SphereMesh createSphereMesh(float radius = 0.5f, unsigned int sectorCount = 32, unsigned int stackCount = 16);

void deleteSphereMesh(const SphereMesh& sphere);

void drawSphere(const SphereMesh& sphere, GLuint modelLoc, const glm::mat4& model);
