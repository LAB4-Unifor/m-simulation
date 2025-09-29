#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "renderer.h"

class GridRenderer {
private:
    GLuint VAO, VBO;
    bool enabled;
    float gridSize;
    int gridDivisions;

public:
    GridRenderer();
    ~GridRenderer();
    void initialize();
    void render(const glm::mat4& view, const glm::mat4& project);
    void setEnabled(bool enable) 
    { 
        enable = enable; 
    }
    
};
