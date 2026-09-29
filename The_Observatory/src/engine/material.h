#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Material
{
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;

    float shininess;

    bool useDiffuseMap;
    bool useSpecularMap;
};

void setMaterial(GLuint shaderProgram, const Material& material);

Material createDullOrangeMaterial();
Material createShinyBlueMaterial();
Material createMatteGrayMaterial();
Material createTexturedMaterial();
