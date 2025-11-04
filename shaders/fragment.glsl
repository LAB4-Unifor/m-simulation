#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 albedo;
uniform vec3 lightPos;  // Add this line

void main()
{
    // Very simple lighting
    // vec3 lightPos = vec3(2.0, 5.0, 2.0);  // Remove hardcoded
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    // Ambient
    vec3 ambient = vec3(0.1) * albedo;
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * albedo;
    
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}