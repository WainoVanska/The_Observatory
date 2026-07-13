#include "mesh.h"

#include <cstddef>

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<MeshTexture>& textures
)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Position)
    );

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Normal)
    );

    // texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, TexCoords)
    );

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

        glVertexAttribPointer(
            location,
            4,
            GL_FLOAT,
            GL_FALSE,
            stride,
            reinterpret_cast<void*>(column * columnSize)
        );

        // INSTANCING UPDATE: Advance this attribute once per instance, not once per vertex
        glVertexAttribDivisor(location, 1);
    }

    glBindVertexArray(0);
}

void Mesh::draw(GLuint shaderProgram) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;

        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(specularNr++);
        }

        std::string uniformName = "material." + name + number;

        glUniform1i(
            glGetUniformLocation(shaderProgram, uniformName.c_str()),
            i
        );

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(indices.size()),
        GL_UNSIGNED_INT,
        0
    );
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::drawInstanced(
    GLuint shaderProgram,
    GLsizei instanceCount
) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;

        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(specularNr++);
        }

        std::string uniformName = "material." + name + number;

        glUniform1i(
            glGetUniformLocation(shaderProgram, uniformName.c_str()),
            i
        );

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);

    glDrawElementsInstanced(
        GL_TRIANGLES,
        static_cast<GLsizei>(indices.size()),
        GL_UNSIGNED_INT,
        nullptr,
        instanceCount
    );

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}
