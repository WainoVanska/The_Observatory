#pragma once

#include <glad/glad.h>
#include <string>

struct FaceUV {
    float u1, v1;
    float u2, v2;
};

struct BoxUVs {
    FaceUV front;
    FaceUV back;
    FaceUV left;
    FaceUV right;
    FaceUV top;
    FaceUV bottom;
};

struct CharacterSkin {
    GLuint texture;

    BoxUVs head;
    BoxUVs body;
    BoxUVs leftArm;
    BoxUVs rightArm;
    BoxUVs leftLeg;
    BoxUVs rightLeg;
};

CharacterSkin loadCharacterSkin(const std::string& texturePath);