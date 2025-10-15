#ifndef GRID_RENDERER_H
#define GRID_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class GridRenderer {
public:
    GridRenderer();
    ~GridRenderer();
    
    bool initialize();
    void render(const glm::mat4& view, const glm::mat4& projection);
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void setGridColor(const glm::vec3& color) { gridColor = color; }
    void setGridAlpha(float alpha) { gridAlpha = alpha; }
    
private:
    GLuint VAO, VBO;
    bool initialized;
    bool enabled;
    glm::vec3 gridColor;
    float gridAlpha;
    GLuint shaderProgram;
    
    bool compileShaders();
};

#endif
