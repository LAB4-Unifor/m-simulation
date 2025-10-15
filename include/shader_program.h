#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();
    
    bool loadFromFiles(const std::string& vertex_path, const std::string& fragment_path);
    void use() const;
    
    // Uniform setters
    void setUniform(const std::string& name, const glm::mat4& matrix) const;
    void setUniform(const std::string& name, const glm::vec3& vector) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, bool value) const;
    
    GLuint getProgramID() const { return program_id; }
    
private:
    GLuint program_id;
    
    std::string readFile(const std::string& file_path);
    GLuint compileShader(GLenum type, const std::string& source);
    GLint getUniformLocation(const std::string& name) const;
};

#endif // SHADER_PROGRAM_H