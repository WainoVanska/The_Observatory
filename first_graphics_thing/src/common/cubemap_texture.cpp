#include "cubemap_texture.h"

#include <iostream>
#include "../../dependencies/stb_image/stb_image.h"

// og tutorial from https://ogldev.org/www/tutorial25/tutorial25.html

static const GLenum types[6] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

CubemapTexture::CubemapTexture(
    const std::string& Directory,
    const std::string& PosXFilename,
    const std::string& NegXFilename,
    const std::string& PosYFilename,
    const std::string& NegYFilename,
    const std::string& PosZFilename,
    const std::string& NegZFilename
)
{
    std::string Dir = Directory;

    if (!Dir.empty() && Dir.back() != '/') {
        Dir += "/";
    }

    m_fileNames[0] = Dir + PosXFilename;
    m_fileNames[1] = Dir + NegXFilename;
    m_fileNames[2] = Dir + PosYFilename;
    m_fileNames[3] = Dir + NegYFilename;
    m_fileNames[4] = Dir + PosZFilename;
    m_fileNames[5] = Dir + NegZFilename;

    m_textureObj = 0;
}

CubemapTexture::~CubemapTexture()
{
    if (m_textureObj != 0) {
        glDeleteTextures(1, &m_textureObj);
    }
}

bool CubemapTexture::Load()
{
    glGenTextures(1, &m_textureObj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);

    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < 6; i++) {
        int width;
        int height;
        int channels;

        unsigned char* data = stbi_load(
            m_fileNames[i].c_str(),
            &width,
            &height,
            &channels,
            0
        );

        if (!data) {
            std::cout << "Failed to load cubemap face: "
                << m_fileNames[i] << std::endl;
            return false;
        }

        GLenum dataFormat = GL_RGB;
        GLenum internalFormat = GL_SRGB;

        if (channels == 1)
        {
            dataFormat = GL_RED;
            internalFormat = GL_RED;
        }
        else if (channels == 3)
        {
            dataFormat = GL_RGB;
            internalFormat = GL_SRGB;
        }
        else if (channels == 4)
        {
            dataFormat = GL_RGBA;
            internalFormat = GL_SRGB_ALPHA;
        }

        glTexImage2D(
            types[i],
            0,
            internalFormat,
            width,
            height,
            0,
            dataFormat,
            GL_UNSIGNED_BYTE,
            data
        );

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

void CubemapTexture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
}