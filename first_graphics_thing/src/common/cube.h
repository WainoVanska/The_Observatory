#pragma once			// tells the compiler that if you've already included this file once, ignore further includes of it

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "character_skin.h"

struct CubeMesh {
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

CubeMesh createCubeMesh();
void deleteCubeMesh(const CubeMesh& cube);

void drawCube(GLuint VAO, GLuint modelLoc, const glm::mat4& model);

void drawTexturedCube(GLuint VAO, GLuint modelLoc, const glm::mat4& model, GLuint texture);

void drawCubeWithUVs(
    const CubeMesh& cube,
    GLuint modelLoc,
    const glm::mat4& model,
    const BoxUVs& uvs
);

void drawSkyboxCube(const CubeMesh& cube);