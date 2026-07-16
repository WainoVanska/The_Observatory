#include "texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stb_image/stb_image.h"

GLuint loadTexture(
    const std::string& path,
    bool useSRGB
)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width = 0;
    int height = 0;
    int nrChannels = 0;

    stbi_set_flip_vertically_on_load(true);

    // Force image to load as RGB so black/white grayscale images
    // do not become red/black GL_RED textures.
    unsigned char* data = stbi_load(
        path.c_str(),
        &width,
        &height,
        &nrChannels,
        3
    );

    if (data)
    {
        std::cout << "Loaded texture: " << path
            << " width=" << width
            << " height=" << height
            << " original channels=" << nrChannels
            << " forced channels=3\n";

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // "do not assume that rows are padded to 4 bytes since I have 250x250 texture"

        const GLenum internalFormat =
            useSRGB
            ? GL_SRGB
            : GL_RGB;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internalFormat,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture: " << path << "\n";
    }

    stbi_image_free(data);

    return texture;
}

GLuint loadTransparentTexture(
    const std::string& path,
    bool useSRGB
)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Important for alpha textures: avoid repeated transparent border artifacts.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0;
    int height = 0;
    int nrChannels = 0;

    stbi_set_flip_vertically_on_load(true);

    // Force RGBA so we definitely keep alpha.
    unsigned char* data = stbi_load(
        path.c_str(),
        &width,
        &height,
        &nrChannels,
        4
    );

    if (data)
    {
        std::cout << "Loaded transparent texture: " << path
            << " width=" << width
            << " height=" << height
            << " original channels=" << nrChannels
            << " forced channels=4\n";

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        const GLenum internalFormat =
            useSRGB
            ? GL_SRGB_ALPHA
            : GL_RGBA;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internalFormat,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load transparent texture: " << path << "\n";
    }

    stbi_image_free(data);

    return texture;
}