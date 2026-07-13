#include "model.h"

#include <iostream>

#include "texture.h"

Model::Model(const std::string& path)
{
    loadModel(path);
}

void Model::draw(GLuint shaderProgram) const
{
    for (const Mesh& mesh : meshes)
    {
        mesh.draw(shaderProgram);
    }
}

void Model::configureInstancing(GLuint instanceVBO)
{
    for (Mesh& mesh : meshes)
    {
        mesh.configureInstanceMatrix(instanceVBO);
    }
}

void Model::drawInstanced(
    GLuint shaderProgram,
    GLsizei instanceCount
) const
{
    for (const Mesh& mesh : meshes)
    {
        mesh.drawInstanced(shaderProgram, instanceCount);
    }
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals
    );

    if (
        !scene ||
        scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode
        )
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    size_t lastSlash = path.find_last_of("/\\");
    directory = path.substr(0, lastSlash);

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};

        vertex.Position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        if (mesh->HasNormals())
        {
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(
            material,
            aiTextureType_DIFFUSE,
            "texture_diffuse"
        );

        textures.insert(
            textures.end(),
            diffuseMaps.begin(),
            diffuseMaps.end()
        );

        std::vector<MeshTexture> specularMaps = loadMaterialTextures(
            material,
            aiTextureType_SPECULAR,
            "texture_specular"
        );

        textures.insert(
            textures.end(),
            specularMaps.begin(),
            specularMaps.end()
        );
    }

    return Mesh(vertices, indices, textures);
}

std::vector<MeshTexture> Model::loadMaterialTextures(
    aiMaterial* material,
    aiTextureType type,
    const std::string& typeName
)
{
    std::vector<MeshTexture> textures;

    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString texturePath;
        material->GetTexture(type, i, &texturePath);

        std::string pathString = texturePath.C_Str();

        bool alreadyLoaded = false;

        for (const MeshTexture& loadedTexture : texturesLoaded)
        {
            if (loadedTexture.path == pathString)
            {
                textures.push_back(loadedTexture);
                alreadyLoaded = true;
                break;
            }
        }

        if (!alreadyLoaded)
        {
            MeshTexture texture{};

            std::string fullPath = directory + "/" + pathString;

            texture.id = loadTexture(fullPath.c_str());
            texture.type = typeName;
            texture.path = pathString;

            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }

    return textures;
}