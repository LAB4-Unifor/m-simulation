#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <GL/glew.h>

class ShaderProgram;

class ModelLoader {
public:
    struct Material {
        glm::vec3 albedo;
        float metallic;
        float roughness;
        float ao;
        
        GLuint albedoMap;
        GLuint normalMap;
        GLuint metallicMap;
        GLuint roughnessMap;
        GLuint aoMap;
        
        bool hasAlbedoMap;
        bool hasNormalMap;
        bool hasMetallicMap;
        bool hasRoughnessMap;
        bool hasAoMap;
        
        Material();
    };
    
    struct Mesh {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        std::vector<unsigned int> indices;
        GLuint VAO, VBO, EBO;
        bool wireframe_enabled = false;
        Material material;
    };
    
    struct Node {
        std::string name;
        glm::mat4 transformation;
        std::vector<unsigned int> meshIndices;
        std::vector<Node> children;
    };

    ModelLoader();
    ~ModelLoader();
    
    bool loadGLTF(const std::string& path);
    void render(const ShaderProgram& shader, const glm::mat4& modelMatrix);
    void enableWireframe(bool enable);
    
    const std::vector<Mesh>& getMeshes() const { return meshes; }
    const Node& getRootNode() const { return rootNode; }
    
private:
    std::vector<Mesh> meshes;
    Node rootNode;
    Assimp::Importer importer;
    bool wireframeMode;
    
    void processNode(aiNode* node, const aiScene* scene, Node& parentNode);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    Material processMaterial(aiMaterial* material, const std::string& directory);
    void setupMesh(Mesh& mesh);
    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
    GLuint loadTexture(const std::string& path);
};

#endif
