 
#include "renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

ShaderProgram::ShaderProgram() : program_id(0) {}

ShaderProgram::~ShaderProgram() {
    if (program_id) {
        glDeleteProgram(program_id);
    }
}

GLuint ShaderProgram::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool ShaderProgram::loadFromFiles(const std::string& vertex_path, const std::string& fragment_path) {
    // Read vertex shader file
    std::ifstream vShaderFile(vertex_path);
    std::string vertexCode;
    if (vShaderFile.is_open()) {
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vertexCode = vShaderStream.str();
        vShaderFile.close();
    } else {
        std::cerr << "Vertex shader file not found: " << vertex_path << std::endl;
        return false;
    }

    // Read fragment shader file
    std::ifstream fShaderFile(fragment_path);
    std::string fragmentCode;
    if (fShaderFile.is_open()) {
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fragmentCode = fShaderStream.str();
        fShaderFile.close();
    } else {
        std::cerr << "Fragment shader file not found: " << fragment_path << std::endl;
        return false;
    }

    // Compile shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
    
    if (!vertexShader || !fragmentShader) {
        return false;
    }

    // Link shaders
    program_id = glCreateProgram();
    glAttachShader(program_id, vertexShader);
    glAttachShader(program_id, fragmentShader);
    glLinkProgram(program_id);
    
    // Check for linking errors
    GLint success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program_id, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program_id);
        program_id = 0;
        return false;
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void ShaderProgram::use() {
    glUseProgram(program_id);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& matrix) {
    GLint location = glGetUniformLocation(program_id, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
    }
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& vector) {
    GLint location = glGetUniformLocation(program_id, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, &vector[0]);
    }
}

void ShaderProgram::setUniform(const std::string& name, float value) {
    GLint location = glGetUniformLocation(program_id, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void ShaderProgram::setUniform(const std::string& name, bool value) {
    GLint location = glGetUniformLocation(program_id, name.c_str());
    if (location != -1) {
        glUniform1i(location, static_cast<int>(value));
    }
}