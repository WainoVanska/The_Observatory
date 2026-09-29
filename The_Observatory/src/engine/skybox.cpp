#include "skybox.h"

#include "shader.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
GLuint createSkyboxShaderProgram()
{
    return createShaderProgram("assets/shaders/skybox/skybox_vertex.glsl",
                               "assets/shaders/skybox/skybox_fragment.glsl");
}
} // namespace

SkyBox::SkyBox(FreeCamera* camera)
    : camera(camera), cubemapTexture(nullptr), cube{}, shaderProgram(0), gWVPLoc(-1), cubemapLoc(-1)
{
}

SkyBox::~SkyBox()
{
    delete cubemapTexture;
    deleteCubeMesh(cube);

    if (shaderProgram != 0)
    {
        glDeleteProgram(shaderProgram);
    }
}

bool SkyBox::Init(const std::string& directory,
                  const std::string& posX,
                  const std::string& negX,
                  const std::string& posY,
                  const std::string& negY,
                  const std::string& posZ,
                  const std::string& negZ)
{
    shaderProgram = createSkyboxShaderProgram();
    gWVPLoc = getUniformLocation(shaderProgram, "gWVP");
    cubemapLoc = getUniformLocation(shaderProgram, "gCubemapTexture");

    glUseProgram(shaderProgram);
    glUniform1i(cubemapLoc, 0);

    cubemapTexture = new CubemapTexture(directory, posX, negX, posY, negY, posZ, negZ);

    if (!cubemapTexture->Load())
    {
        std::cout << "Failed to load cubemap texture\n";
        return false;
    }

    cube = createCubeMesh();
    return true;
}

void SkyBox::Render(const glm::mat4& projection)
{
    glUseProgram(shaderProgram);

    GLint oldCullFaceMode;
    GLint oldDepthFuncMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    const glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));
    const glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix()));
    const glm::mat4 wvp = projection * view * model;
    glUniformMatrix4fv(gWVPLoc, 1, GL_FALSE, glm::value_ptr(wvp));

    cubemapTexture->Bind(GL_TEXTURE0);
    glBindVertexArray(cube.VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glCullFace(oldCullFaceMode);
    glDepthFunc(oldDepthFuncMode);
}

void SkyBox::BindTexture(GLenum textureUnit)
{
    if (cubemapTexture)
    {
        cubemapTexture->Bind(textureUnit);
    }
}
