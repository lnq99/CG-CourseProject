#pragma one

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "material.h"


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
