#include "lighting_system.h"
#include <iostream>

LightingSystem::Light::Light() 
    : position(0.0f), color(1.0f), intensity(1.0f), enabled(true) {}

LightingSystem::Light::Light(const glm::vec3& pos, const glm::vec3& col, float intens) 
    : position(pos), color(col), intensity(intens), enabled(true) {}

LightingSystem::LightingSystem() 
    : ambientColor(0.1f, 0.1f, 0.1f), ambientIntensity(0.1f), usePBR(true) {}

void LightingSystem::addLight(const Light& light) {
    if (lights.size() < 4) {
        lights.push_back(light);
    } else {
        std::cerr << "Maximum number of lights reached!" << std::endl;
    }
}

void LightingSystem::removeLight(int index) {
    if (index >= 0 && index < lights.size()) {
        lights.erase(lights.begin() + index);
    }
}

LightingSystem::Light& LightingSystem::getLight(int index) {
    static Light defaultLight;
    if (index >= 0 && index < lights.size()) {
        return lights[index];
    }
    return defaultLight;
}

void LightingSystem::setAmbientLight(const glm::vec3& color, float intensity) {
    ambientColor = color;
    ambientIntensity = intensity;
}

void LightingSystem::applyToShader(ShaderProgram& shader) {
    shader.use();
    
    shader.setUniform("numLights", static_cast<int>(lights.size()));
    
    for (size_t i = 0; i < lights.size(); i++) {
        std::string prefix = "lights[" + std::to_string(i) + "]";
        shader.setUniform(prefix + ".position", lights[i].position);
        shader.setUniform(prefix + ".color", lights[i].color);
        shader.setUniform(prefix + ".intensity", lights[i].intensity);
        shader.setUniform(prefix + ".enabled", lights[i].enabled);
    }
    
    shader.setUniform("ambientColor", ambientColor);
    shader.setUniform("ambientIntensity", ambientIntensity);
    shader.setUniform("usePBR", usePBR);
}
