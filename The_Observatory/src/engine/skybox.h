#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "cubemap_texture.h"
#include "free_camera.h"
#include "cube.h"

class SkyBox
{
public:
    SkyBox(FreeCamera* camera);
    ~SkyBox();

    bool Init(
        const std::string& directory,
        const std::string& posX,
        const std::string& negX,
        const std::string& posY,
        const std::string& negY,
        const std::string& posZ,
        const std::string& negZ
    );

    void Render(const glm::mat4& projection);
    void BindTexture(GLenum textureUnit);

private:
    FreeCamera* camera;
    CubemapTexture* cubemapTexture;

    CubeMesh cube;

    GLuint shaderProgram;
    GLint gWVPLoc;
    GLint cubemapLoc;
};
