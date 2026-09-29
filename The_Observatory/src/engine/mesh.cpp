#include "mesh.h"

#include "shader.h"

#include <cstddef>
#include <utility>

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           const std::vector<MeshTexture>& textures,
           bool isTransparent,
           float opacity)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->isTransparent = isTransparent;
    this->opacity = opacity;

    setupMesh();
}

Mesh::~Mesh()
{
    releaseGpuResources();
}

Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices)), indices(std::move(other.indices)), textures(std::move(other.textures)),
      VAO(std::exchange(other.VAO, 0)), VBO(std::exchange(other.VBO, 0)), EBO(std::exchange(other.EBO, 0)),
      isTransparent(other.isTransparent), opacity(other.opacity)
{
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    releaseGpuResources();
    vertices = std::move(other.vertices);
    indices = std::move(other.indices);
    textures = std::move(other.textures);
    VAO = std::exchange(other.VAO, 0);
    VBO = std::exchange(other.VBO, 0);
    EBO = std::exchange(other.EBO, 0);
    isTransparent = other.isTransparent;
    opacity = other.opacity;
    return *this;
}

void Mesh::releaseGpuResources()
{
    if (EBO != 0)
    {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::configureInstanceMatrix(GLuint instanceVBO)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    constexpr GLsizei stride = sizeof(glm::mat4);
    constexpr std::size_t columnSize = sizeof(glm::vec4);

    for (GLuint column = 0; column < 4; column++)
    {
        GLuint location = 3 + column;

        glEnableVertexAttribArray(location);

        glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(column * columnSize));

        // INSTANCING UPDATE: Advance this attribute once per instance, not once per vertex
        glVertexAttribDivisor(location, 1);
    }

    glBindVertexArray(0);
}

void Mesh::draw(GLuint shaderProgram) const
{
    bindMaterial(shaderProgram);

    GLboolean blendingWasEnabled = GL_FALSE;
    GLboolean depthWritingWasEnabled = GL_TRUE;
    beginTransparentDraw(blendingWasEnabled, depthWritingWasEnabled);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    endTransparentDraw(blendingWasEnabled, depthWritingWasEnabled);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::drawInstanced(GLuint shaderProgram, GLsizei instanceCount) const
{
    bindMaterial(shaderProgram);

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES,
                            static_cast<GLsizei>(indices.size()),
                            GL_UNSIGNED_INT,
                            nullptr,
                            instanceCount);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::bindMaterial(GLuint shaderProgram) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int emissiveNr = 1;

    bool hasDiffuseMap = false;
    bool hasSpecularMap = false;
    bool hasEmissiveMap = false;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        const std::string& name = textures[i].type;

        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
            hasDiffuseMap = true;
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(specularNr++);
            hasSpecularMap = true;
        }
        else if (name == "texture_emissive")
        {
            number = std::to_string(emissiveNr++);
            hasEmissiveMap = true;
        }

        std::string uniformName = "material." + name + number;

        glUniform1i(getUniformLocation(shaderProgram, uniformName), static_cast<GLint>(i));

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glUniform1i(getUniformLocation(shaderProgram, "material.hasDiffuseMap"), hasDiffuseMap);

    glUniform1i(getUniformLocation(shaderProgram, "material.hasSpecularMap"), hasSpecularMap);

    glUniform1i(getUniformLocation(shaderProgram, "material.hasEmissiveMap"), hasEmissiveMap);

    glUniform1f(getUniformLocation(shaderProgram, "material.opacity"), opacity);
}

void Mesh::beginTransparentDraw(GLboolean& blendingWasEnabled, GLboolean& depthWritingWasEnabled) const
{
    blendingWasEnabled = glIsEnabled(GL_BLEND);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWritingWasEnabled);
    if (isTransparent)
    {
        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthMask(GL_FALSE);
    }
}

void Mesh::endTransparentDraw(GLboolean blendingWasEnabled, GLboolean depthWritingWasEnabled) const
{
    if (isTransparent)
    {
        glDepthMask(depthWritingWasEnabled);

        if (!blendingWasEnabled)
        {
            glDisable(GL_BLEND);
        }
    }
}
