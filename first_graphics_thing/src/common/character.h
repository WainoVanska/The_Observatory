#pragma once			// tells the compiler that if you've already included this file once, ignore further includes of it

#include <glad/glad.h>
#include <glm/glm.hpp>

void drawCharacter(
    GLuint VAO,
    GLuint modelLoc,
    float t,
    const glm::mat4& baseTransform
);
