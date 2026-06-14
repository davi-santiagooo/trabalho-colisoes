#version 330 core

in vec3 aPos;
in vec3 aNormal;

out vec4 Normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    mat4 transform = projection * view * model;
    gl_Position = transform * vec4(aPos, 1.0);
    fragPos = vec3(model * vec4(aPos, 1.0));
    Normal = vec4(aNormal, 1.0);
}