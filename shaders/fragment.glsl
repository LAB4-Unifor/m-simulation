#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool useTexture;
};

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    bool enabled;
};

uniform Material material;
uniform Light lights[4];
uniform int numLights;
uniform vec3 ambientColor;
uniform float ambientIntensity;
uniform bool wireframeMode;
uniform bool usePBR; // Add this uniform

void main() {
    if (wireframeMode) {
        FragColor = vec4(0.8, 0.8, 0.8, 1.0);
        return;
    }

    vec3 ambient = ambientColor * ambientIntensity * material.ambient;
    vec3 result = ambient;

    for (int i = 0; i < numLights; i++) {
        if (!lights[i].enabled) continue;

        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lights[i].color * diff * material.diffuse * lights[i].intensity;

        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = lights[i].color * spec * material.specular * lights[i].intensity;

        result += diffuse + specular;
    }

    FragColor = vec4(result, 1.0);
}