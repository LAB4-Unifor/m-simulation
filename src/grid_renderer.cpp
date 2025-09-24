#include "grid_renderer.h"
#include <vector>
#include <iostream>

GridRenderer::GridRenderer() 
    : VAO(0), VBO(0), enabled(true), gridSize(10.0f), gridDivisions(20) {}

GridRenderer::~GridRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void GridRenderer::initialize() {
    // Generate grid vertices (X and Z axes)
    std::vector<glm::vec3> vertices;
    float step = gridSize / gridDivisions;
    
    // X-axis lines
    for (int i = -gridDivisions; i <= gridDivisions; ++i) {
        float x = i * step;
        vertices.push_back(glm::vec3(x, 0.0f, -gridSize));
        vertices.push_back(glm::vec3(x, 0.0f, gridSize));
    }
    
    // Z-axis lines
    for (int i = -gridDivisions; i <= gridDivisions; ++i) {
        float z = i * step;
        vertices.push_back(glm::vec3(-gridSize, 0.0f, z));
        vertices.push_back(glm::vec3(gridSize, 0.0f, z));
    }

    // Create VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void GridRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!enabled) return;
    
    // Use grid shader
    static ShaderProgram gridShader;
    static bool initialized = false;
    
    if (!initialized) {
        if (!gridShader.loadFromFiles("shaders/grid_vertex.glsl", "shaders/grid_fragment.glsl")) {
            std::cerr << "Failed to load grid shaders" << std::endl;
            return;
        }
        initialized = true;
    }
    
    gridShader.use();
    
    // Set uniforms
    glm::mat4 model = glm::mat4(1.0f);
    gridShader.setUniform("model", model);
    gridShader.setUniform("view", view);
    gridShader.setUniform("projection", projection);
    
    // Render grid
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}