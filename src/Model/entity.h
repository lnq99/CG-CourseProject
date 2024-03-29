#pragma once

#include "material.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


struct Sphere {
    Material material;
    glm::vec3 pos;
    float radius;
};

struct Plane {
    Material material;
    glm::vec3 normal;
    float distance;
};

struct Triangle {
    glm::vec3 v0;
    int32_t _pad0;
    glm::vec3 v1;
    int32_t _pad1;
    glm::vec3 v2;
    int32_t _pad2;
};

struct AABB {
    Material material;
    glm::vec3 bMin;
    int32_t _pad0;
    glm::vec3 bMax;
    int32_t _pad1;
};
