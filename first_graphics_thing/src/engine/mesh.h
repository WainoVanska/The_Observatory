#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct MeshTexture
{
    GLuint id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<MeshTexture>& textures,
        bool isTransparent = false,
        float opacity = 1.0f
    );

    void draw(GLuint shaderProgram) const;

    void configureInstanceMatrix(GLuint instanceVBO);
    void drawInstanced(GLuint shaderProgram, GLsizei instanceCount) const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    bool isTransparent = false;
    float opacity = 1.0f;

    void setupMesh();
};