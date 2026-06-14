#include "skybox.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static std::string readShaderFileSkybox(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint createSkyboxShaderProgram()
{
    std::string vertexCode = readShaderFileSkybox("src/common/skybox_vertex_shader.glsl");
    std::string fragmentCode = readShaderFileSkybox("src/common/skybox_fragment_shader.glsl");

    std::cout << "Skybox vertex shader size: " << vertexCode.size() << std::endl;
    std::cout << "Skybox fragment shader size: " << fragmentCode.size() << std::endl;

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

SkyBox::SkyBox(FreeCamera* camera)
    : camera(camera),
    cubemapTexture(nullptr),
    cube{},
    shaderProgram(0),
    gWVPLoc(-1),
    cubemapLoc(-1)
{
}

SkyBox::~SkyBox()
{
    delete cubemapTexture;

    deleteCubeMesh(cube);

    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
    }
}

bool SkyBox::Init(
    const std::string& directory,
    const std::string& posX,
    const std::string& negX,
    const std::string& posY,
    const std::string& negY,
    const std::string& posZ,
    const std::string& negZ
)
{
    shaderProgram = createSkyboxShaderProgram();

    gWVPLoc = glGetUniformLocation(shaderProgram, "gWVP");              //??
    cubemapLoc = glGetUniformLocation(shaderProgram, "gCubemapTexture");

    glUseProgram(shaderProgram);
    glUniform1i(cubemapLoc, 0);

    cubemapTexture = new CubemapTexture(
        directory,
        posX,
        negX,
        posY,
        negY,
        posZ,
        negZ
    );

    if (!cubemapTexture->Load()) {
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

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(20.0f));

    glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix()));

    glm::mat4 wvp = projection * view * model;

    glUniformMatrix4fv(gWVPLoc, 1, GL_FALSE, glm::value_ptr(wvp));

    cubemapTexture->Bind(GL_TEXTURE0);

    glBindVertexArray(cube.VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glCullFace(oldCullFaceMode);
    glDepthFunc(oldDepthFuncMode);
}