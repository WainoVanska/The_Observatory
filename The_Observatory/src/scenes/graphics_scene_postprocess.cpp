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

void GraphicsScene::createFramebuffer(int width, int height)
{
    framebufferWidth = width;
    framebufferHeight = height;

    glGenFramebuffers(1, &multisampleFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, multisampleFramebuffer);

    glGenTextures(1, &multisampleColorTexture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleColorTexture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB16F, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleColorTexture, 0);

    glGenRenderbuffers(1, &multisampleRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, multisampleRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampleRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Multisample framebuffer is not complete!\n";
    }

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

    glGenTextures(1, &framebufferDepthTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferDepthTexture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH24_STENCIL8,
                 width,
                 height,
                 0,
                 GL_DEPTH_STENCIL,
                 GL_UNSIGNED_INT_24_8,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebufferDepthTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Resolved framebuffer is not complete!\n";
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsScene::createBloomFramebuffer(int width, int height)
{
    glGenFramebuffers(1, &brightPassFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, brightPassFBO);

    glGenTextures(1, &brightTexture);
    glBindTexture(GL_TEXTURE_2D, brightTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brightTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Bright-pass "
                  << "framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsScene::createPingPongFramebuffers(int width, int height)
{
    glGenFramebuffers(2, pingPongFBO.data());

    glGenTextures(2, pingPongTextures.data());

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);

        glBindTexture(GL_TEXTURE_2D, pingPongTextures[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTextures[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Ping-pong " << i << " framebuffer is not complete!" << std::endl;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsScene::extractBrightAreas()
{
    glBindFramebuffer(GL_FRAMEBUFFER, brightPassFBO);

    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(brightPassShaderProgram);

    glUniform1f(brightPassUniforms.brightnessThreshold, 1.0f);

    bindTexture(GL_TEXTURE0, GL_TEXTURE_2D, framebufferTexture);

    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void GraphicsScene::blurBrightAreas()
{
    constexpr unsigned int blurPassCount = 10;

    bool horizontal = true;
    bool firstPass = true;

    glUseProgram(gaussianBlurShaderProgram);

    glBindVertexArray(screenQuadVAO);

    for (unsigned int pass = 0; pass < blurPassCount; pass++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[horizontal]);

        glUniform1i(gaussianBlurUniforms.horizontal, horizontal);

        GLuint inputTexture;

        if (firstPass)
        {
            inputTexture = brightTexture;
        }
        else
        {
            inputTexture = pingPongTextures[!horizontal];
        }

        bindTexture(GL_TEXTURE0, GL_TEXTURE_2D, inputTexture);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        horizontal = !horizontal;
        firstPass = false;
    }

    glBindVertexArray(0);

    blurredBloomTexture = pingPongTextures[!horizontal];

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsScene::createScreenQuad()
{
    // clang-format off
    const float vertices[] = {
        // Position     UV
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
    };
    // clang-format on

    glGenVertexArrays(1, &screenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);
    glBindVertexArray(screenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    configureVertexAttribute(0, 2, 4 * sizeof(float), 0);
    configureVertexAttribute(1, 2, 4 * sizeof(float), 2 * sizeof(float));
    glBindVertexArray(0);
}

void GraphicsScene::drawScreenQuad()
{
    glUseProgram(screenShaderProgram);

    glUniform1i(screenUniforms.screenTexture, 0);
    glUniform1i(screenUniforms.bloomTexture, 1);
    glUniform1i(screenUniforms.depthTexture, 2);

    glUniform1i(screenUniforms.blurEnabled, blurEnabled);
    glUniform1i(screenUniforms.gammaCorrectionEnabled, true);
    glUniform1i(screenUniforms.hdrEnabled, true);
    glUniform1f(screenUniforms.exposure, 1.2f);
    glUniform1i(screenUniforms.bloomEnabled, bloomEnabled);
    glUniform1f(screenUniforms.bloomStrength, bloomStrength);

    glUniform1i(screenUniforms.depthDebugEnabled, depthDebugEnabled);
    glUniform1f(screenUniforms.nearPlane, 0.1f);
    glUniform1f(screenUniforms.farPlane, 100.0f);

    bindTexture(GL_TEXTURE0, GL_TEXTURE_2D, framebufferTexture);
    bindTexture(GL_TEXTURE1, GL_TEXTURE_2D, blurredBloomTexture);
    bindTexture(GL_TEXTURE2, GL_TEXTURE_2D, framebufferDepthTexture);

    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
