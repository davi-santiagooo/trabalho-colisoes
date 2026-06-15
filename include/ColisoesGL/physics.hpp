#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <glm/ext/vector_float3.hpp>
#include <vector>

void col_CreateUvSphere(int sectors, int layers, float radius, std::vector<float>& points, std::vector<int>& indices, std::vector<float>& normals);

typedef struct Ball Ball;

typedef struct Ball {
    glm::vec3 pos;
    glm::vec3 vel;
    float radius;
} Ball;

#endif