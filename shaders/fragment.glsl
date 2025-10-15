#version 430 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool useTexture;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool enabled;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform bool wireframeMode = false;
uniform vec3 wireframeColor = vec3(0.0, 1.0, 0.0);
uniform bool lightingEnabled = true;

void main() {
    if (wireframeMode) {
        FragColor = vec4(wireframeColor, 1.0);
        return;
    }
    
    if (!lightingEnabled) {
        FragColor = vec4(material.diffuse, 1.0);
        return;
    }
    
    vec3 ambient = light.ambient * material.ambient;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
