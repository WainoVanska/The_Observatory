#include "graphics_scene.h"
#include "graphics_scene_support.h"

#include "../engine/shader.h"
#include "../engine/texture.h"

#include <array>
#include <cstddef>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace graphics_scene_detail;

void GraphicsScene::createDirectionalShadowMap()
{
    glGenFramebuffers(1, &directionalDepthFBO);
    glGenTextures(1, &directionalDepthMap);
    glBindTexture(GL_TEXTURE_2D, directionalDepthMap);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT24,
        SHADOW_WIDTH,
        SHADOW_HEIGHT,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, directionalDepthFBO);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        directionalDepthMap,
        0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Directional shadow framebuffer incomplete\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 GraphicsScene::createDirectionalLightSpaceMatrix(const glm::vec3& direction) const
{
    const glm::vec3 sceneCenter(0.0f);
    const glm::vec3 lightPosition = sceneCenter - glm::normalize(direction) * 10.0f;
    const glm::mat4 lightProjection = glm::ortho(-7.0f, 7.0f, -7.0f, 7.0f, 0.1f, 25.0f);
    const glm::mat4 lightView = glm::lookAt(
        lightPosition,
        sceneCenter,
        glm::vec3(0.0f, 1.0f, 0.0f));
    return lightProjection * lightView;
}

void GraphicsScene::drawDirectionalShadowCasters(
    GLuint depthShaderProgram,
    const glm::mat4& lightSpaceMatrix)
{
    (void)depthShaderProgram;
    (void)lightSpaceMatrix;
    const GLint modelLocation = directionalDepthUniforms.model;
    drawCube(cube.VAO, modelLocation, createOutdoorGroundModelMatrix());
    drawCube(
        cube.VAO,
        modelLocation,
        makeTransform(glm::vec3(-0.2f, -0.6f, 0.0f), glm::vec3(1.0f)));
    drawSphere(
        sphere,
        modelLocation,
        makeTransform(glm::vec3(0.8f, -0.4f, 0.0f), glm::vec3(1.0f)));

    if (backpack)
    {
        setMatrix(modelLocation, createBackpackModelMatrix());
        backpack->draw(depthShaderProgram);
    }

    if (rock)
    {
        glUseProgram(instancedDirectionalDepthShaderProgram);
        setMatrix(
            instancedDirectionalDepthUniforms.lightSpaceMatrix,
            createDirectionalLightSpaceMatrix(sunDirection()));
        setMatrix(instancedDirectionalDepthUniforms.systemModel, createAsteroidSystemModel());
        rock->drawInstanced(instancedDirectionalDepthShaderProgram, ASTEROID_COUNT);
        glUseProgram(depthShaderProgram);
    }
}

void GraphicsScene::renderDirectionalShadowMap(const glm::mat4& lightSpaceMatrix)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, directionalDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(directionalDepthShaderProgram);
    setMatrix(directionalDepthUniforms.lightSpaceMatrix, lightSpaceMatrix);
    glCullFace(GL_FRONT);
    drawDirectionalShadowCasters(directionalDepthShaderProgram, lightSpaceMatrix);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsScene::createPointShadowMaps()
{
    glGenFramebuffers(
        1,
        &pointDepthFBO
    );

    glGenTextures(
        static_cast<GLsizei>(
            pointDepthCubemaps.size()
            ),
        pointDepthCubemaps.data()
    );

    for (GLuint cubemap : pointDepthCubemaps)
    {
        glBindTexture(
            GL_TEXTURE_CUBE_MAP,
            cubemap
        );

        for (unsigned int face = 0;
            face < 6;
            ++face)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X +
                face,
                0,
                GL_DEPTH_COMPONENT24,
                POINT_SHADOW_WIDTH,
                POINT_SHADOW_HEIGHT,
                0,
                GL_DEPTH_COMPONENT,
                GL_FLOAT,
                nullptr
            );
        }

        glTexParameteri(
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_MIN_FILTER,
            GL_NEAREST
        );

        glTexParameteri(
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_MAG_FILTER,
            GL_NEAREST
        );

        glTexParameteri(
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_WRAP_R,
            GL_CLAMP_TO_EDGE
        );
    }

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        pointDepthFBO
    );

    glFramebufferTexture(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        pointDepthCubemaps[0],
        0
    );

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (
        glCheckFramebufferStatus(
            GL_FRAMEBUFFER
        ) != GL_FRAMEBUFFER_COMPLETE
        )
    {
        std::cerr
            << "Point shadow framebuffer incomplete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    fixedPointShadowMapsValid.fill(false);
}

std::vector<glm::mat4> GraphicsScene::createPointShadowMatrices(
    const glm::vec3& lightPosition) const
{
    const float aspect = static_cast<float>(POINT_SHADOW_WIDTH) /
        static_cast<float>(POINT_SHADOW_HEIGHT);
    const glm::mat4 projection = glm::perspective(
        glm::radians(90.0f),
        aspect,
        POINT_SHADOW_NEAR_PLANE,
        POINT_SHADOW_FAR_PLANE);

    const std::array<glm::vec3, 6> directions = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    const std::array<glm::vec3, 6> upVectors = {
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f)
    };

    std::vector<glm::mat4> matrices;
    matrices.reserve(6);
    for (std::size_t face = 0; face < directions.size(); ++face)
    {
        matrices.push_back(projection * glm::lookAt(
            lightPosition,
            lightPosition + directions[face],
            upVectors[face]));
    }
    return matrices;
}

void GraphicsScene::drawPointShadowCasters(GLuint depthShaderProgram)
{
    (void)depthShaderProgram;
    const GLint modelLocation = pointDepthUniforms.model;
    drawCube(
        cube.VAO,
        modelLocation,
        makeTransform(glm::vec3(-0.2f, -0.6f, 0.0f), glm::vec3(1.0f)));
    drawSphere(
        sphere,
        modelLocation,
        makeTransform(glm::vec3(0.8f, -0.4f, 0.0f), glm::vec3(1.0f)));

    if (backpack)
    {
        setMatrix(modelLocation, createBackpackModelMatrix());
        backpack->draw(depthShaderProgram);
    }
}

void GraphicsScene::renderPointShadowMap(
    const glm::vec3& lightPosition,
    GLuint targetCubemap)
{
    const std::vector<glm::mat4>
        shadowMatrices =
        createPointShadowMatrices(
            lightPosition
        );

    glViewport(
        0,
        0,
        POINT_SHADOW_WIDTH,
        POINT_SHADOW_HEIGHT
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        pointDepthFBO
    );

    glFramebufferTexture(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        targetCubemap,
        0
    );

    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(pointDepthShaderProgram);

    for (std::size_t face = 0; face < pointDepthUniforms.shadowMatrices.size(); ++face)
    {
        setMatrix(pointDepthUniforms.shadowMatrices[face], shadowMatrices[face]);
    }

    setVector(pointDepthUniforms.lightPosition, lightPosition);

    glUniform1f(pointDepthUniforms.farPlane, POINT_SHADOW_FAR_PLANE);

    const GLboolean cullingWasEnabled =
        glIsEnabled(GL_CULL_FACE);

    GLint previousCullFaceMode = GL_BACK;

    glGetIntegerv(
        GL_CULL_FACE_MODE,
        &previousCullFaceMode
    );

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    drawPointShadowCasters(
        pointDepthShaderProgram
    );

    glCullFace(
        static_cast<GLenum>(
            previousCullFaceMode
            )
    );

    if (!cullingWasEnabled)
    {
        glDisable(GL_CULL_FACE);
    }

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

void GraphicsScene::configureModelPointShadows()
{
    bindPointShadowMaps(
        modelUniforms,
        pointDepthCubemaps
    );

    glUniform1f(
        modelUniforms.pointShadowFarPlane,
        POINT_SHADOW_FAR_PLANE
    );

    glUniform1i(
        modelUniforms.pointShadowsEnabled,
        !outdoorModeEnabled
    );
}
