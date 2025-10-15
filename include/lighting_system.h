#ifndef LIGHTING_SYSTEM_H
#define LIGHTING_SYSTEM_H

#include <vector>
#include <glm/glm.hpp>
#include "shader_program.h"

class LightingSystem {
public:
    struct Light {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        bool enabled;
        
        Light();
        Light(const glm::vec3& pos, const glm::vec3& col, float intens);
    };

    LightingSystem();
    
    void addLight(const Light& light);
    void removeLight(int index);
    Light& getLight(int index);
    int getLightCount() const { return lights.size(); }
    
    void setAmbientLight(const glm::vec3& color, float intensity);
    void setUsePBR(bool use) { usePBR = use; }
    
    void applyToShader(ShaderProgram& shader);
    
private:
    std::vector<Light> lights;
    glm::vec3 ambientColor;
    float ambientIntensity;
    bool usePBR;
};

#endif
