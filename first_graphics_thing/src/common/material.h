#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

void setMaterial(GLuint shaderProgram, const Material& material);

Material createDullOrangeMaterial();
Material createShinyBlueMaterial();
Material createMatteGrayMaterial();