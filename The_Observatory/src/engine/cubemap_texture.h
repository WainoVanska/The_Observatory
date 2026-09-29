#pragma once

#include <string>
#include <glad/glad.h>

class CubemapTexture
{
  public:
    CubemapTexture(const std::string& Directory,
                   const std::string& PosXFilename,
                   const std::string& NegXFilename,
                   const std::string& PosYFilename,
                   const std::string& NegYFilename,
                   const std::string& PosZFilename,
                   const std::string& NegZFilename);

    ~CubemapTexture();

    bool Load();

    void Bind(GLenum TextureUnit);

  private:
    std::string m_fileNames[6];
    GLuint m_textureObj;
};
