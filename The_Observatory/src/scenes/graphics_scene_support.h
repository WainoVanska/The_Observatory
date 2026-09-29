#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <cstddef>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../engine/cube.h"
#include "../engine/model.h"
#include "../engine/shader.h"
#include "../engine/sphere.h"

namespace graphics_scene_detail
{
constexpr float FLASHLIGHT_INNER_ANGLE = 10.0f;
constexpr float FLASHLIGHT_OUTER_ANGLE = 28.0f;
constexpr std::size_t ROOM_POINT_LIGHT_COUNT = 4;

inline const glm::vec3 DISTANT_PLANET_POSITION(0.0f, 6.0f, -80.0f);
constexpr float DISTANT_PLANET_SCALE = 12.0f;
constexpr float DISTANT_ASTEROID_SYSTEM_SCALE = 1.20f;

inline const std::array<glm::vec3, ROOM_POINT_LIGHT_COUNT> ROOM_POINT_LIGHT_POSITIONS = {
    glm::vec3(-2.60f, 1.30f, -1.35f),
    glm::vec3(-2.60f, 1.30f, 1.35f),
    glm::vec3(2.60f, 1.30f, -1.35f),
    glm::vec3(2.60f, 1.30f, 1.35f)};

inline const glm::vec3 FIXED_ROOM_LIGHT_COLOR(0.18f, 0.16f, 0.12f);
inline const glm::vec3 MOVABLE_LIGHT_COLOR(0.70f, 0.63f, 0.50f);

inline void setMatrix(GLint location, const glm::mat4& value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

inline void setVector(GLint location, const glm::vec3& value)
{
    glUniform3fv(location, 1, glm::value_ptr(value));
}

inline void bindTexture(GLenum unit, GLenum target, GLuint texture)
{
    glActiveTexture(unit);
    glBindTexture(target, texture);
}

inline glm::mat4 makeTransform(const glm::vec3& position,
                               const glm::vec3& scale,
                               float angleDegrees = 0.0f,
                               const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f))
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    if (angleDegrees != 0.0f)
    {
        model = glm::rotate(model, glm::radians(angleDegrees), axis);
    }
    return glm::scale(model, scale);
}

inline glm::vec3 sunDirection()
{
    return glm::vec3(-0.3f, -1.0f, -0.2f);
}

inline glm::vec3 sunColor()
{
    return glm::vec3(0.45f, 0.43f, 0.36f);
}

template <typename Uniforms> void cacheRoomPointLightUniforms(GLuint program, Uniforms& uniforms)
{
    for (std::size_t i = 0; i < ROOM_POINT_LIGHT_COUNT; ++i)
    {
        const std::string prefix = "pointLights[" + std::to_string(i) + "].";
        uniforms.pointLightPositions[i] = getUniformLocation(program, prefix + "position");
        uniforms.pointLightColors[i] = getUniformLocation(program, prefix + "color");
        uniforms.pointLightEnabled[i] = getUniformLocation(program, prefix + "enabled");
    }
}

template <typename Uniforms> void cachePointShadowMapUniforms(GLuint program, Uniforms& uniforms)
{
    for (std::size_t i = 0; i < ROOM_POINT_LIGHT_COUNT; ++i)
    {
        uniforms.pointShadowMaps[i] = getUniformLocation(program, "pointShadowMap" + std::to_string(i));
    }
}

template <typename Uniforms>
void bindPointShadowMaps(const Uniforms& uniforms, const std::array<GLuint, ROOM_POINT_LIGHT_COUNT>& cubemaps)
{
    for (std::size_t i = 0; i < ROOM_POINT_LIGHT_COUNT; ++i)
    {
        bindTexture(GL_TEXTURE4 + static_cast<GLenum>(i), GL_TEXTURE_CUBE_MAP, cubemaps[i]);
        glUniform1i(uniforms.pointShadowMaps[i], 4 + static_cast<GLint>(i));
    }
}

template <typename Uniforms>
void uploadRoomPointLights(const Uniforms& uniforms,
                           const glm::vec3& movableLightPosition,
                           bool movableModeEnabled,
                           bool outdoorModeEnabled)
{
    for (std::size_t i = 0; i < ROOM_POINT_LIGHT_COUNT; ++i)
    {
        const bool enabled = !outdoorModeEnabled && (!movableModeEnabled || i == 0);
        glm::vec3 position = ROOM_POINT_LIGHT_POSITIONS[i];
        if (movableModeEnabled && i == 0)
        {
            position = movableLightPosition;
        }

        setVector(uniforms.pointLightPositions[i], position);
        const glm::vec3 color = movableModeEnabled && i == 0 ? MOVABLE_LIGHT_COLOR : FIXED_ROOM_LIGHT_COLOR;
        setVector(uniforms.pointLightColors[i], color);
        glUniform1i(uniforms.pointLightEnabled[i], enabled);
    }
}

inline bool toggleOnKeyPress(GLFWwindow* window, int key, bool& wasPressed, bool& value)
{
    const bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
    const bool toggled = isPressed && !wasPressed;
    if (toggled)
    {
        value = !value;
    }
    wasPressed = isPressed;
    return toggled;
}

inline void configureVertexAttribute(GLuint index, GLint componentCount, GLsizei stride, std::size_t byteOffset)
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, componentCount, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(byteOffset));
}

inline void drawDebugPrimitives(const CubeMesh& cube, const SphereMesh& sphere, GLint modelLocation, GLuint texture)
{
    drawTexturedCube(cube.VAO, modelLocation, makeTransform(glm::vec3(-0.2f, -0.6f, 0.0f), glm::vec3(1.0f)), texture);
    drawSphere(sphere, modelLocation, makeTransform(glm::vec3(0.8f, -0.4f, 0.0f), glm::vec3(1.0f)));
}

template <typename Uniforms, typename Context, typename Lighting>
void configureLighting(const Uniforms& uniforms, const Context& context, const Lighting& lighting)
{
    setMatrix(uniforms.view, context.view);
    setMatrix(uniforms.projection, context.projection);
    setVector(uniforms.viewPos, context.cameraPosition);
    setVector(uniforms.lightPos, lighting.primaryPosition);
    setVector(uniforms.lightColor, lighting.primaryColor);
    setVector(uniforms.secondLightPos, lighting.secondaryPosition);
    setVector(uniforms.secondLightColor, lighting.secondaryColor);
    glUniform1i(uniforms.light1Enabled, lighting.primaryEnabled);
    glUniform1i(uniforms.light2Enabled, lighting.secondaryEnabled);
    glUniform1i(uniforms.directionalLightEnabled, lighting.directionalEnabled);
    setVector(uniforms.directionalLightDirection, sunDirection());
    setVector(uniforms.directionalLightColor, sunColor());
    setVector(uniforms.flashlightPos, context.cameraPosition);
    setVector(uniforms.flashlightDir, context.cameraFront);
    glUniform1i(uniforms.flashlightEnabled, lighting.flashlightEnabled);
    glUniform1f(uniforms.flashlightCutOff, glm::cos(glm::radians(FLASHLIGHT_INNER_ANGLE)));
    glUniform1f(uniforms.flashlightOuterCutOff, glm::cos(glm::radians(FLASHLIGHT_OUTER_ANGLE)));
}

template <typename Uniforms>
void drawModelWithMatrices(Model* model,
                           GLuint program,
                           const Uniforms& uniforms,
                           const glm::mat4& view,
                           const glm::mat4& projection,
                           const glm::mat4& modelMatrix)
{
    if (!model)
    {
        return;
    }

    glUseProgram(program);
    setMatrix(uniforms.view, view);
    setMatrix(uniforms.projection, projection);
    setMatrix(uniforms.model, modelMatrix);
    model->draw(program);
}

struct TangentBasis
{
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

inline TangentBasis calculateTangentBasis(const glm::vec3& p1,
                                          const glm::vec3& p2,
                                          const glm::vec3& p3,
                                          const glm::vec2& uv1,
                                          const glm::vec2& uv2,
                                          const glm::vec2& uv3)
{
    const glm::vec3 edge1 = p2 - p1;
    const glm::vec3 edge2 = p3 - p1;
    const glm::vec2 deltaUV1 = uv2 - uv1;
    const glm::vec2 deltaUV2 = uv3 - uv1;
    const float inverseDeterminant = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    TangentBasis basis;
    basis.tangent = glm::normalize(inverseDeterminant * (deltaUV2.y * edge1 - deltaUV1.y * edge2));
    basis.bitangent = glm::normalize(inverseDeterminant * (-deltaUV2.x * edge1 + deltaUV1.x * edge2));
    return basis;
}
} // namespace graphics_scene_detail
