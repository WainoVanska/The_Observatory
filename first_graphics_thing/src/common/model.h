#pragma once

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

#include "mesh.h"

class Model
{
public:
    Model(const std::string& path);

    void draw(GLuint shaderProgram) const;
    void configureInstancing(GLuint instanceVBO);
    void drawInstanced(GLuint shaderProgram, GLsizei instanceCount) const;

private:
    std::vector<Mesh> meshes;
    std::vector<MeshTexture> texturesLoaded;

    std::string directory;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<MeshTexture> loadMaterialTextures(
        aiMaterial* material,
        aiTextureType type,
        const std::string& typeName
    );
};