#include "material.h"

#include <glm/gtc/type_ptr.hpp>

void setMaterial(GLuint shaderProgram, const Material& material)
{
    const glm::vec3 linearDiffuseColor =
        glm::pow(
            material.diffuseColor,
            glm::vec3(2.2f)
        );

    glUniform3fv(
        glGetUniformLocation(
            shaderProgram,
            "material.diffuseColor"
        ),
        1,
        glm::value_ptr(linearDiffuseColor)
    );

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "material.specularColor"),
        1,
        &material.specularColor[0]
    );

    glUniform1f(
        glGetUniformLocation(shaderProgram, "material.shininess"),
        material.shininess
    );

    glUniform1i(
        glGetUniformLocation(shaderProgram, "material.useDiffuseMap"),
        material.useDiffuseMap
    );

    glUniform1i(
        glGetUniformLocation(shaderProgram, "material.useSpecularMap"),
        material.useSpecularMap
    );

    glUniform1i(
        glGetUniformLocation(shaderProgram, "material.diffuseMap"),
        0
    );

    glUniform1i(
        glGetUniformLocation(shaderProgram, "material.specularMap"),
        1
    );
}

Material createDullOrangeMaterial()
{
    Material material{};

    material.diffuseColor = glm::vec3(0.90f, 0.40f, 0.20f);
    material.specularColor = glm::vec3(0.10f, 0.10f, 0.10f);
    material.shininess = 8.0f;
    material.useDiffuseMap = false;
    material.useSpecularMap = false;

    return material;
}

Material createShinyBlueMaterial()
{
    Material material{};

    material.diffuseColor = glm::vec3(0.20f, 0.50f, 1.00f);
    material.specularColor = glm::vec3(0.80f, 0.80f, 0.80f);
    material.shininess = 64.0f;
    material.useDiffuseMap = false;
    material.useSpecularMap = false;

    return material;
}

Material createMatteGrayMaterial()
{
    Material material{};

    material.diffuseColor = glm::vec3(0.70f, 0.70f, 0.70f);
    material.specularColor = glm::vec3(0.00f, 0.00f, 0.00f);
    material.shininess = 1.0f;
    material.useDiffuseMap = false;
    material.useSpecularMap = false;

    return material;
}

Material createTexturedMaterial()
{
    Material material{};

    material.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    material.specularColor = glm::vec3(0.25f, 0.25f, 0.25f);
    material.shininess = 32.0f;
    material.useDiffuseMap = true;
    material.useSpecularMap = true;

    return material;
}