#include "model_loader.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

ModelLoader::ModelLoader() : directory("") {}

ModelLoader::~ModelLoader() {
    for (auto& mesh : meshes) {
        if (mesh.VAO) glDeleteVertexArrays(1, &mesh.VAO);
        if (mesh.VBO) glDeleteBuffers(1, &mesh.VBO);
        if (mesh.EBO) glDeleteBuffers(1, &mesh.EBO);
    }
}

bool ModelLoader::loadGLTF(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
    return true;
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

ModelLoader::Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
    Mesh newMesh;
    
    // Extract vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 vertex;
        vertex.x = mesh->mVertices[i].x;
        vertex.y = mesh->mVertices[i].y;
        vertex.z = mesh->mVertices[i].z;
        newMesh.vertices.push_back(vertex);
        
        // Extract normals
        if (mesh->mNormals) {
            glm::vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;
            newMesh.normals.push_back(normal);
        }
        
        // Extract texture coordinates
        if (mesh->mTextureCoords[0]) {
            glm::vec2 texCoord;
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;
            newMesh.texCoords.push_back(texCoord);
        }
    }
    
    // Extract indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            newMesh.indices.push_back(face.mIndices[j]);
    }
    
    // Generate VAO/VBO
    setupMeshBuffers(newMesh);
    return newMesh;
}

void ModelLoader::setupMeshBuffers(Mesh& mesh) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    
    glBindVertexArray(mesh.VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                mesh.vertices.size() * sizeof(glm::vec3), 
                &mesh.vertices[0], 
                GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                mesh.indices.size() * sizeof(unsigned int),
                &mesh.indices[0],
                GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    if (!mesh.normals.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);
    }
    
    // Texture coordinate attribute
    if (!mesh.texCoords.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
    }
    
    glBindVertexArray(0);
}

void ModelLoader::enableWireframe(bool enable) {
    for (auto& mesh : meshes) {
        mesh.wireframe_enabled = enable;
    }
}

void ModelLoader::render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, bool wireframe) {
    for (auto& mesh : meshes) {
        if (mesh.wireframe_enabled && !wireframe) continue;
        
        glBindVertexArray(mesh.VAO);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        
        // Set model-view-projection matrix
        glm::mat4 mvp = projection * view * model;
        
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}