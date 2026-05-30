#version 330 core

out vec4 outColor;

uniform vec3 objColor;

void main() {
    outColor = vec4(objColor, 1.0);
}