#version 430 core

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

out vec4 FragColor;

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
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
uniform vec3 viewPos;
uniform bool usePBR;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.albedo, material.metallic);
    
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numLights; ++i) {
        if (!lights[i].enabled) continue;
        
        vec3 L = normalize(lights[i].position - FragPos);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lights[i].color * lights[i].intensity * attenuation;
        
        float NDF = DistributionGGX(N, H, material.roughness);
        float G = GeometrySmith(N, V, L, material.roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.metallic;
        
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * material.albedo / PI + specular) * radiance * NdotL;
    }
    
    vec3 ambient = vec3(0.03) * material.albedo * material.ao;
    vec3 color = ambient + Lo;
    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}
