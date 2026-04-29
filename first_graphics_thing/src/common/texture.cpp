#include "texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stb_image/stb_image.h"

GLuint loadTexture(const std::string& path)
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

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
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