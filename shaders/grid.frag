#version 430 core

out vec4 FragColor;

uniform vec3 gridColor = vec3(0.5, 0.5, 0.5);
uniform float gridAlpha = 0.5;

void main() {
    FragColor = vec4(gridColor, gridAlpha);
}
