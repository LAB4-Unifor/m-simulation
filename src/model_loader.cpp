#include "model_loader.h"
#include <iostream>
#include "shader_program.h"

ModelLoader::Material::Material() 
    : albedo(0.7f, 0.7f, 0.7f),
      metallic(0.0f),
      roughness(0.5f),
      ao(1.0f),
      albedoMap(0),
      normalMap(0),
      metallicMap(0),
      roughnessMap(0),
      aoMap(0),
      hasAlbedoMap(false),
      hasNormalMap(false),
      hasMetallicMap(false),
      hasRoughnessMap(false),
      hasAoMap(false) {}

ModelLoader::ModelLoader() : wireframeMode(false) {}

ModelLoader::~ModelLoader() {
    for (auto& mesh : meshes) {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        if (mesh.indices.size() > 0) {
            glDeleteBuffers(1, &mesh.EBO);
        }
    }
}

bool ModelLoader::loadGLTF(const std::string& path) {
    std::cout << "=== ASSIMP MODEL LOADING DEBUG ===" << std::endl;
    std::cout << "Loading model with path: " << path << std::endl;
    
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | 
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);
    
    if (!scene) {
        std::cerr << "✗ Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }
    
    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "✗ Assimp error: Scene is incomplete" << std::endl;
        return false;
    }
    
    if (!scene->mRootNode) {
        std::cerr << "✗ Assimp error: No root node" << std::endl;
        return false;
    }
    
    std::cout << "✓ Scene loaded successfully!" << std::endl;
    std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
    std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;
    std::cout << "Number of animations: " << scene->mNumAnimations << std::endl;
    
    rootNode.name = scene->mRootNode->mName.C_Str();
    std::cout << "Root node name: " << rootNode.name << std::endl;
    
    rootNode.transformation = aiMatrix4x4ToGlm(scene->mRootNode->mTransformation);
    processNode(scene->mRootNode, scene, rootNode);
    
    std::cout << "✓ Total meshes processed: " << meshes.size() << std::endl;
    std::cout << "✓ Model loaded successfully!" << std::endl;
    std::cout << "=== END ASSIMP DEBUG ===" << std::endl;
    return true;
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene, Node& parentNode) {
    std::cout << "Processing node: " << node->mName.C_Str() 
              << " with " << node->mNumMeshes << " meshes" 
              << " and " << node->mNumChildren << " children" << std::endl;
    
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "  Processing mesh " << i << ": " << mesh->mName.C_Str() 
                  << " with " << mesh->mNumVertices << " vertices" << std::endl;
        
        meshes.push_back(processMesh(mesh, scene));
        parentNode.meshIndices.push_back(meshes.size() - 1);
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        Node childNode;
        childNode.name = node->mChildren[i]->mName.C_Str();
        childNode.transformation = aiMatrix4x4ToGlm(node->mChildren[i]->mTransformation);
        processNode(node->mChildren[i], scene, childNode);
        parentNode.children.push_back(childNode);
    }
}

ModelLoader::Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
    Mesh result;
    
    std::cout << "    Mesh details:" << std::endl;
    std::cout << "      - Vertices: " << mesh->mNumVertices << std::endl;
    std::cout << "      - Faces: " << mesh->mNumFaces << std::endl;
    std::cout << "      - Has normals: " << (mesh->HasNormals() ? "YES" : "NO") << std::endl;
    std::cout << "      - Has texture coords: " << (mesh->mTextureCoords[0] ? "YES" : "NO") << std::endl;
    
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        result.vertices.push_back(vector);
        
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            result.normals.push_back(vector);
        }
        
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            result.texCoords.push_back(vec);
        } else {
            result.texCoords.push_back(glm::vec2(0.0f, 0.0f));
        }
    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            result.indices.push_back(face.mIndices[j]);
        }
    }
    
    std::cout << "      - Processed " << result.vertices.size() << " vertices" << std::endl;
    std::cout << "      - Processed " << result.indices.size() << " indices" << std::endl;
    
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        result.material = processMaterial(material, "");
        std::cout << "      - Material processed" << std::endl;
    }
    
    setupMesh(result);
    return result;
}

ModelLoader::Material ModelLoader::processMaterial(aiMaterial* material, const std::string& directory) {
    Material mat;
    
    aiColor3D color(0.f, 0.f, 0.f);
    float value;
    
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        mat.albedo = glm::vec3(color.r, color.g, color.b);
    }
    
    if (material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
        mat.metallic = value;
    }
    
    if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
        mat.roughness = value;
    }
    
    return mat;
}

void ModelLoader::setupMesh(Mesh& mesh) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    
    if (mesh.indices.size() > 0) {
        glGenBuffers(1, &mesh.EBO);
    }
    
    glBindVertexArray(mesh.VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 mesh.vertices.size() * sizeof(glm::vec3) + 
                 mesh.normals.size() * sizeof(glm::vec3) +
                 mesh.texCoords.size() * sizeof(glm::vec2),
                 NULL, GL_STATIC_DRAW);
    
    GLsizeiptr offset = 0;
    glBufferSubData(GL_ARRAY_BUFFER, offset, 
                    mesh.vertices.size() * sizeof(glm::vec3), 
                    mesh.vertices.data());
    offset += mesh.vertices.size() * sizeof(glm::vec3);
    
    glBufferSubData(GL_ARRAY_BUFFER, offset, 
                    mesh.normals.size() * sizeof(glm::vec3), 
                    mesh.normals.data());
    offset += mesh.normals.size() * sizeof(glm::vec3);
    
    glBufferSubData(GL_ARRAY_BUFFER, offset, 
                    mesh.texCoords.size() * sizeof(glm::vec2), 
                    mesh.texCoords.data());
    
    if (mesh.indices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     mesh.indices.size() * sizeof(unsigned int), 
                     mesh.indices.data(), GL_STATIC_DRAW);
    }
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 
                         (void*)(mesh.vertices.size() * sizeof(glm::vec3)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 
                         (void*)(mesh.vertices.size() * sizeof(glm::vec3) + 
                                 mesh.normals.size() * sizeof(glm::vec3)));
    
    glBindVertexArray(0);
}

void ModelLoader::render(const ShaderProgram& shader, const glm::mat4& modelMatrix) {
    shader.setUniform("model", modelMatrix);
    
    for (const auto& mesh : meshes) {
        // Set material properties in a way that works with both shaders
        shader.setUniform("albedo", mesh.material.albedo);
        shader.setUniform("material.diffuse", mesh.material.albedo);
        shader.setUniform("material.ambient", mesh.material.albedo * 0.2f);
        shader.setUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setUniform("material.shininess", 32.0f);
        shader.setUniform("material.useTexture", false);
        
        shader.setUniform("metallic", mesh.material.metallic);
        shader.setUniform("roughness", mesh.material.roughness);
        shader.setUniform("ao", mesh.material.ao);
        
        shader.setUniform("wireframeMode", mesh.wireframe_enabled || wireframeMode);
        
        glBindVertexArray(mesh.VAO);
        if (mesh.indices.size() > 0) {
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
        }
        glBindVertexArray(0);
    }
}

void ModelLoader::enableWireframe(bool enable) {
    wireframeMode = enable;
}

glm::mat4 ModelLoader::aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

GLuint ModelLoader::loadTexture(const std::string& path) {
    std::cout << "Loading texture: " << path << std::endl;
    return 0;
}
