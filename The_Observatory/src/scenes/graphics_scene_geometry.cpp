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

glm::mat4 GraphicsScene::createOutdoorGroundModelMatrix() const
{
    return makeTransform(glm::vec3(0.0f, -0.95f, 0.0f), glm::vec3(20.0f, 0.1f, 20.0f));
}

glm::mat4 GraphicsScene::createBackpackOutlineModelMatrix() const
{
    return makeTransform(glm::vec3(-1.0f, -0.5f, 0.0f), glm::vec3(0.215f), 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 GraphicsScene::createBackpackModelMatrix() const
{
    return makeTransform(glm::vec3(-1.0f, -0.5f, 0.0f), glm::vec3(0.205f), 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void GraphicsScene::createGlassPaneMesh()
{
    // clang-format off
    const float vertices[] = {
        // Position              UV
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,

        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
    };
    // clang-format on

    glGenVertexArrays(1, &glassVAO);
    glGenBuffers(1, &glassVBO);
    glBindVertexArray(glassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, glassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    configureVertexAttribute(0, 3, 5 * sizeof(float), 0);
    configureVertexAttribute(1, 2, 5 * sizeof(float), 3 * sizeof(float));
    glBindVertexArray(0);
}

void GraphicsScene::createAsteroidInstances()
{
    if (!rock)
    {
        return;
    }

    asteroidMatrices.resize(ASTEROID_COUNT);
    std::mt19937 randomEngine(1337);
    std::uniform_real_distribution<float> displacement(-8.0f, 8.0f);
    std::uniform_real_distribution<float> scaleDistribution(0.05f, 0.12f);
    std::uniform_real_distribution<float> rotationDistribution(0.0f, glm::two_pi<float>());
    const glm::vec3 rotationAxis = glm::normalize(glm::vec3(0.4f, 0.6f, 0.8f));
    constexpr float radius = 50.0f;

    for (unsigned int i = 0; i < ASTEROID_COUNT; ++i)
    {
        const float percentage = static_cast<float>(i) / static_cast<float>(ASTEROID_COUNT);
        const float angle = percentage * glm::two_pi<float>();
        const float x = std::sin(angle) * radius + displacement(randomEngine);
        const float y = displacement(randomEngine) * 0.25f;
        const float z = std::cos(angle) * radius + displacement(randomEngine);
        const glm::vec3 position(x, y, z);
        const float scale = scaleDistribution(randomEngine);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, glm::vec3(scale));
        asteroidMatrices[i] = glm::rotate(model, rotationDistribution(randomEngine), rotationAxis);
    }

    glGenBuffers(1, &asteroidInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, asteroidInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, asteroidMatrices.size() * sizeof(glm::mat4), asteroidMatrices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    rock->configureInstancing(asteroidInstanceVBO);
}

glm::mat4 GraphicsScene::createAsteroidSystemModel() const
{
    const float orbitAngle = static_cast<float>(glfwGetTime()) * glm::radians(3.0f);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), DISTANT_PLANET_POSITION);

    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    model = glm::rotate(model, orbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    return glm::scale(model, glm::vec3(DISTANT_ASTEROID_SYSTEM_SCALE));
}

void GraphicsScene::createNormalMappedPlane()
{
    const glm::vec3 p1(-1.0f, 1.0f, 0.0f);
    const glm::vec3 p2(1.0f, 1.0f, 0.0f);
    const glm::vec3 p3(1.0f, -1.0f, 0.0f);
    const glm::vec3 p4(-1.0f, -1.0f, 0.0f);
    const glm::vec2 uv1(0.0f, 1.0f);
    const glm::vec2 uv2(1.0f, 1.0f);
    const glm::vec2 uv3(1.0f, 0.0f);
    const glm::vec2 uv4(0.0f, 0.0f);
    const glm::vec3 normal(0.0f, 0.0f, 1.0f);
    const TangentBasis first = calculateTangentBasis(p1, p2, p3, uv1, uv2, uv3);
    const TangentBasis second = calculateTangentBasis(p1, p3, p4, uv1, uv3, uv4);

    // clang-format off
    const float vertices[] = {
        // Position         Normal                          UV           Tangent / bitangent
        // First triangle
        p1.x, p1.y, p1.z,  normal.x, normal.y, normal.z,  uv1.x, uv1.y,
        first.tangent.x, first.tangent.y, first.tangent.z,
        first.bitangent.x, first.bitangent.y, first.bitangent.z,

        p2.x, p2.y, p2.z,  normal.x, normal.y, normal.z,  uv2.x, uv2.y,
        first.tangent.x, first.tangent.y, first.tangent.z,
        first.bitangent.x, first.bitangent.y, first.bitangent.z,

        p3.x, p3.y, p3.z,  normal.x, normal.y, normal.z,  uv3.x, uv3.y,
        first.tangent.x, first.tangent.y, first.tangent.z,
        first.bitangent.x, first.bitangent.y, first.bitangent.z,

        // Second triangle
        p1.x, p1.y, p1.z,  normal.x, normal.y, normal.z,  uv1.x, uv1.y,
        second.tangent.x, second.tangent.y, second.tangent.z,
        second.bitangent.x, second.bitangent.y, second.bitangent.z,

        p3.x, p3.y, p3.z,  normal.x, normal.y, normal.z,  uv3.x, uv3.y,
        second.tangent.x, second.tangent.y, second.tangent.z,
        second.bitangent.x, second.bitangent.y, second.bitangent.z,

        p4.x, p4.y, p4.z,  normal.x, normal.y, normal.z,  uv4.x, uv4.y,
        second.tangent.x, second.tangent.y, second.tangent.z,
        second.bitangent.x, second.bitangent.y, second.bitangent.z,
    };
    // clang-format on

    glGenVertexArrays(1, &normalMappedPlaneVAO);
    glGenBuffers(1, &normalMappedPlaneVBO);
    glBindVertexArray(normalMappedPlaneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalMappedPlaneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    constexpr GLsizei stride = 14 * sizeof(float);
    configureVertexAttribute(0, 3, stride, 0);
    configureVertexAttribute(1, 3, stride, 3 * sizeof(float));
    configureVertexAttribute(2, 2, stride, 6 * sizeof(float));
    configureVertexAttribute(3, 3, stride, 8 * sizeof(float));
    configureVertexAttribute(4, 3, stride, 11 * sizeof(float));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GraphicsScene::drawNormalMappedSurface(const glm::mat4& model,
                                            const glm::vec2& uvScale,
                                            GLuint diffuseTexture,
                                            GLuint normalTexture,
                                            GLuint depthTexture,
                                            float normalStrength,
                                            float heightScale,
                                            const RenderContext& context,
                                            const LightingState& lighting)
{
    glUseProgram(normalMappingShaderProgram);

    glUniform1i(normalMappingUniforms.normalDebugEnabled, normalDebugEnabled ? GL_TRUE : GL_FALSE);

    bindPointShadowMaps(normalMappingUniforms, pointDepthCubemaps);
    glUniform1f(normalMappingUniforms.pointShadowFarPlane, POINT_SHADOW_FAR_PLANE);
    glUniform1i(normalMappingUniforms.pointShadowsEnabled, !outdoorModeEnabled);
    uploadRoomPointLights(normalMappingUniforms, lighting.primaryPosition, movableLightModeEnabled, outdoorModeEnabled);
    setMatrix(normalMappingUniforms.model, model);
    setMatrix(normalMappingUniforms.view, context.view);
    setMatrix(normalMappingUniforms.projection, context.projection);
    setVector(normalMappingUniforms.viewPos, context.cameraPosition);
    setVector(normalMappingUniforms.lightPos, lighting.primaryPosition);
    setVector(normalMappingUniforms.lightColor, lighting.primaryColor);
    glUniform1i(normalMappingUniforms.lightEnabled, !outdoorModeEnabled && movableLightModeEnabled);
    glUniform1i(normalMappingUniforms.wallLightEnabled, !outdoorModeEnabled && !movableLightModeEnabled);
    glUniform2f(normalMappingUniforms.uvScale, uvScale.x, uvScale.y);
    glUniform1f(normalMappingUniforms.normalStrength, normalStrength);
    glUniform1f(normalMappingUniforms.heightScale, heightScale);
    bindTexture(GL_TEXTURE0, GL_TEXTURE_2D, diffuseTexture);
    bindTexture(GL_TEXTURE1, GL_TEXTURE_2D, normalTexture);
    bindTexture(GL_TEXTURE2, GL_TEXTURE_2D, depthTexture);

    const GLboolean cullingWasEnabled = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glBindVertexArray(normalMappedPlaneVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    if (cullingWasEnabled)
    {
        glEnable(GL_CULL_FACE);
    }
    glActiveTexture(GL_TEXTURE0);
}

void GraphicsScene::drawNormalMappedRoomWalls(const RenderContext& context, const LightingState& lighting)
{
    struct Wall
    {
        glm::vec3 position;
        float rotation;
    };

    constexpr float roomWallPosition = 2.95f;

    const std::array<Wall, 3> walls = {Wall{glm::vec3(0.0f, 1.0f, roomWallPosition), 180.0f},
                                       Wall{glm::vec3(-roomWallPosition, 1.0f, 0.0f), 90.0f},
                                       Wall{glm::vec3(roomWallPosition, 1.0f, 0.0f), -90.0f}};

    for (const Wall& wall : walls)
    {
        drawNormalMappedSurface(makeTransform(wall.position, glm::vec3(3.0f, 2.0f, 1.0f), wall.rotation),
                                glm::vec2(4.0f, 3.0f),
                                brickDiffuseTexture,
                                brickNormalTexture,
                                brickDepthTexture,
                                1.0f,
                                parallaxHeightScale,
                                context,
                                lighting);
    }
}

void GraphicsScene::drawNormalMappedFloorAndCeiling(const RenderContext& context, const LightingState& lighting)
{
    struct Surface
    {
        glm::vec3 position;
        float rotation;
        glm::vec2 uvScale;
        float normalStrength;
    };

    const std::array<Surface, 2> surfaces = {Surface{glm::vec3(0.0f, -0.95f, 0.0f), -90.0f, glm::vec2(3.0f), 0.9f},
                                             Surface{glm::vec3(0.0f, 2.95f, 0.0f), 90.0f, glm::vec2(2.0f), 0.3f}};

    for (const Surface& surface : surfaces)
    {
        drawNormalMappedSurface(
            makeTransform(surface.position, glm::vec3(3.0f, 3.0f, 1.0f), surface.rotation, glm::vec3(1.0f, 0.0f, 0.0f)),
            surface.uvScale,
            brickDiffuseTexture,
            brickNormalTexture,
            brickDepthTexture,
            surface.normalStrength,
            parallaxHeightScale,
            context,
            lighting);
    }
}
