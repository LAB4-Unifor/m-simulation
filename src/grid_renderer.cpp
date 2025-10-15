#include "grid_renderer.h"
#include <iostream>
#include <vector>

GridRenderer::GridRenderer() 
    : initialized(false), enabled(true), gridColor(0.5f, 0.5f, 0.5f), gridAlpha(0.5f), shaderProgram(0) {}

GridRenderer::~GridRenderer() {
    if (initialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        if (shaderProgram != 0) {
            glDeleteProgram(shaderProgram);
        }
    }
}

bool GridRenderer::initialize() {
    if (initialized) {
        return true;
    }
    
    if (!compileShaders()) {
        std::cerr << "Failed to compile grid shaders!" << std::endl;
        return false;
    }
    
    std::vector<glm::vec3> vertices;
    float size = 10.0f;
    int divisions = 20;
    float step = size / divisions;
    
    for (int i = -divisions; i <= divisions; ++i) {
        float x = i * step;
        vertices.push_back(glm::vec3(x, 0.0f, -size));
        vertices.push_back(glm::vec3(x, 0.0f, size));
    }
    
    for (int i = -divisions; i <= divisions; ++i) {
        float z = i * step;
        vertices.push_back(glm::vec3(-size, 0.0f, z));
        vertices.push_back(glm::vec3(size, 0.0f, z));
    }
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    initialized = true;
    return true;
}

bool GridRenderer::compileShaders() {
    const char* vertexShaderSource = R"glsl(
        #version 430 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * vec4(aPos, 1.0);
        }
    )glsl";
    
    const char* fragmentShaderSource = R"glsl(
        #version 430 core
        out vec4 FragColor;
        uniform vec3 gridColor;
        uniform float gridAlpha;
        void main() {
            FragColor = vec4(gridColor, gridAlpha);
        }
    )glsl";
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Grid vertex shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return false;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Grid fragment shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Grid shader program linking failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void GridRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!initialized || !enabled) {
        return;
    }
    
    glUseProgram(shaderProgram);
    
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "gridColor");
    GLint alphaLoc = glGetUniformLocation(shaderProgram, "gridAlpha");
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    glUniform3f(colorLoc, gridColor.r, gridColor.g, gridColor.b);
    glUniform1f(alphaLoc, gridAlpha);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 84);
    
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}
