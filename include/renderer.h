#pragma once
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

class ShaderProgram {
private:
    GLuint program_id;
    
    // Helper function to compile shader
    GLuint compileShader(GLenum type, const std::string& source);

public:
    ShaderProgram();
    ~ShaderProgram();
    
    bool loadFromFiles(const std::string& vertex_path, const std::string& fragment_path);
    void use();
    
    // Uniform setters
    void setUniform(const std::string& name, const glm::mat4& matrix);
    void setUniform(const std::string& name, const glm::vec3& vector);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, bool value);
};