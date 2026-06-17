#include "material.h"

#include <glm/gtc/type_ptr.hpp>

void setMaterial(GLuint shaderProgram, const Material& material)
{
    glUniform3fv(
        glGetUniformLocation(shaderProgram, "material.ambient"),
        1,
        glm::value_ptr(material.ambient)
    );

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "material.diffuse"),
        1,
        glm::value_ptr(material.diffuse)
    );

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "material.specular"),
        1,
        glm::value_ptr(material.specular)
    );

    glUniform1f(
        glGetUniformLocation(shaderProgram, "material.shininess"),
        material.shininess
    );
}

Material createDullOrangeMaterial()
{
    Material material;

    material.ambient = glm::vec3(0.20f, 0.08f, 0.03f);
    material.diffuse = glm::vec3(0.90f, 0.40f, 0.20f);
    material.specular = glm::vec3(0.10f, 0.10f, 0.10f);
    material.shininess = 8.0f;

    return material;
}

Material createShinyBlueMaterial()
{
    Material material;

    material.ambient = glm::vec3(0.02f, 0.05f, 0.10f);
    material.diffuse = glm::vec3(0.20f, 0.50f, 1.00f);
    material.specular = glm::vec3(0.80f, 0.80f, 0.80f);
    material.shininess = 64.0f;

    return material;
}

Material createMatteGrayMaterial()
{
    Material material;

    material.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
    material.diffuse = glm::vec3(0.70f, 0.70f, 0.70f);
    material.specular = glm::vec3(0.00f, 0.00f, 0.00f);
    material.shininess = 1.0f;

    return material;
}