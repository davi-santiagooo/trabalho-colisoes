#version 330 core

in vec4 Normal;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(Normal).xyz;

    fragColor = vec4(norm.xyz, 1.0);
}