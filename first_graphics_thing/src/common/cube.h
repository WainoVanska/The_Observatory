#pragma once

#include <glad/glad.h>

struct CubeMesh {
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

CubeMesh createCubeMesh();
void deleteCubeMesh(const CubeMesh& cube);