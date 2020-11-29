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
    int32_t id;                                // Id used to identify sphere for raytracing
    float radius;
    glm::ivec3 _pad;
};

// SSBO plane declaration
struct Plane {
    Material material;
    glm::vec3 normal;
    int32_t id;
    float distance;
    glm::ivec3 _pad;
};
