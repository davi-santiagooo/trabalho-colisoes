#version 330 core

in vec4 Normal;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 objColor;
uniform vec3 camPos;

void main() {
    vec3 norm = normalize(Normal).xyz;

    vec3 lightPos = vec3(0.0, 0.0, 2.0);
    vec3 lightDir = normalize(lightPos - fragPos);

    vec3 camPos = lightPos;
    vec3 camDir = normalize(camPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float ambientStrength = 0.1;
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 ambient =  lightColor * ambientStrength;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    float spec = pow(max(dot(camDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objColor;

    fragColor = vec4(norm.xyz, 1.0);
}