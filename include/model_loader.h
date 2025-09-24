#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ModelLoader {
public:
    struct Mesh {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<unsigned int> indices;
        GLuint VAO, VBO, EBO;
        bool wireframe_enabled = false;
    };

    ModelLoader();
    ~ModelLoader();
    bool loadGLTF(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void enableWireframe(bool enable);
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, bool wireframe = false);
    const std::vector<Mesh>& getMeshes() const { return meshes; }

private:
    std::vector<Mesh> meshes;
    std::string directory;
};

void setupMeshBuffers(ModelLoader::Mesh& mesh);