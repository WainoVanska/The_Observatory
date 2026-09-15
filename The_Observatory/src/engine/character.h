#pragma once			// tells the compiler that if you've already included this file once, ignore further includes of it

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "character_skin.h"
#include "cube.h"

void drawCharacter(
    const CubeMesh& cube,
    GLuint modelLoc,
    float t,
    const glm::mat4& baseTransform,
    const CharacterSkin& skin
);
