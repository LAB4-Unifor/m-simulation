#include "shader_program.h"
#include <iostream>
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram() : program_id(0) {}

ShaderProgram::~ShaderProgram() {
    if (program_id != 0) {
        glDeleteProgram(program_id);
    }
}

bool ShaderProgram::loadFromFiles(const std::string& vertex_path, const std::string& fragment_path) {
    std::string vertex_source = readFile(vertex_path);
    std::string fragment_source = readFile(fragment_path);
    
    if (vertex_source.empty() || fragment_source.empty()) {
        std::cerr << "Failed to read shader files!" << std::endl;
        return false;
    }
    
    GLuint vertex_shader = compileShader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compileShader(GL_FRAGMENT_SHADER, fragment_source);
    
    if (vertex_shader == 0 || fragment_shader == 0) {
        return false;
    }
    
    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);
    
    GLint success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar info_log[512];
        glGetProgramInfoLog(program_id, 512, nullptr, info_log);
        std::cerr << "Shader program linking failed: " << info_log << std::endl;
        
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glDeleteProgram(program_id);
        program_id = 0;
        
        return false;
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return true;
}

void ShaderProgram::use() const {
    glUseProgram(program_id);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& matrix) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
    }
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& vector) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3f(location, vector.x, vector.y, vector.z);
    }
}

void ShaderProgram::setUniform(const std::string& name, float value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void ShaderProgram::setUniform(const std::string& name, int value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void ShaderProgram::setUniform(const std::string& name, bool value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value ? 1 : 0);
    }
}

std::string ShaderProgram::readFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint ShaderProgram::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* source_cstr = source.c_str();
    glShaderSource(shader, 1, &source_cstr, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::cerr << "Shader compilation failed: " << info_log << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLint ShaderProgram::getUniformLocation(const std::string& name) const {
    GLint location = glGetUniformLocation(program_id, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader!" << std::endl;
    }
    return location;
}