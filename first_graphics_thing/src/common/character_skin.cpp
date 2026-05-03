#include "character_skin.h"
#include "texture.h"

// Converts pixel coordinates into normalized OpenGL UV coordinates.
// Example: texture size 64x64, x = 8 becomes u = 8 / 64 = 0.125
static FaceUV uv(float x, float y, float w, float h, float texW, float texH)
{
    FaceUV result;

    result.u1 = x / texW;
    result.u2 = (x + w) / texW;

    result.v1 = 1.0f - (y + h) / texH;
    result.v2 = 1.0f - y / texH;

    return result;
}

CharacterSkin loadCharacterSkin(const std::string& texturePath)
{
    CharacterSkin skin;
    skin.texture = loadTexture(texturePath);

    // Classic Minecraft skin size
    const float texW = 64.0f;
    const float texH = 64.0f;

    // Head UVs
    skin.head.top = uv(8, 0, 8, 8, texW, texH);
    skin.head.bottom = uv(16, 0, 8, 8, texW, texH);
    skin.head.right = uv(0, 8, 8, 8, texW, texH);
    skin.head.front = uv(8, 8, 8, 8, texW, texH);
    skin.head.left = uv(16, 8, 8, 8, texW, texH);
    skin.head.back = uv(24, 8, 8, 8, texW, texH);

    // Body UVs
    skin.body.top = uv(20, 16, 8, 4, texW, texH);
    skin.body.bottom = uv(28, 16, 8, 4, texW, texH);
    skin.body.right = uv(16, 20, 4, 12, texW, texH);
    skin.body.front = uv(20, 20, 8, 12, texW, texH);
    skin.body.left = uv(28, 20, 4, 12, texW, texH);
    skin.body.back = uv(32, 20, 8, 12, texW, texH);

    // Right arm, Minecraft classic layout
    skin.rightArm.top = uv(44, 16, 4, 4, texW, texH);
    skin.rightArm.bottom = uv(48, 16, 4, 4, texW, texH);
    skin.rightArm.right = uv(40, 20, 4, 12, texW, texH);
    skin.rightArm.front = uv(44, 20, 4, 12, texW, texH);
    skin.rightArm.left = uv(48, 20, 4, 12, texW, texH);
    skin.rightArm.back = uv(52, 20, 4, 12, texW, texH);

    // Right leg
    skin.rightLeg.top = uv(4, 16, 4, 4, texW, texH);
    skin.rightLeg.bottom = uv(8, 16, 4, 4, texW, texH);
    skin.rightLeg.right = uv(0, 20, 4, 12, texW, texH);
    skin.rightLeg.front = uv(4, 20, 4, 12, texW, texH);
    skin.rightLeg.left = uv(8, 20, 4, 12, texW, texH);
    skin.rightLeg.back = uv(12, 20, 4, 12, texW, texH);

    // mirror the arms and legs
    skin.leftArm = skin.rightArm;
    skin.leftLeg = skin.rightLeg;

    return skin;
}