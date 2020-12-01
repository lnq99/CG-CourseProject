#pragma one

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "material.h"


// SSBO sphere declaration
struct Sphere {                                    // Shader uses std140 layout (so we only use vec4 instead of vec3)
    Material material;
    glm::vec3 pos;
    float radius;
};

// SSBO plane declaration
struct Plane {
    Material material;
    glm::vec3 normal;
    float distance;
};

// SSBO triangle declaration
struct Triangle {
    // Material material;
    glm::vec3 v0;
    int32_t _pad0;
    glm::vec3 v1;
    int32_t _pad1;
    glm::vec3 v2;
    int32_t _pad2;
};
