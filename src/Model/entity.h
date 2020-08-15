#pragma one

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


// SSBO sphere declaration
struct Sphere {                                    // Shader uses std140 layout (so we only use vec4 instead of vec3)
    glm::vec3 pos;
    float radius;
    glm::vec3 diffuse;
    float specular;
    uint32_t id;                                // Id used to identify sphere for raytracing
    glm::ivec3 _pad;
};

// SSBO plane declaration
struct Plane {
    glm::vec3 normal;
    float distance;
    glm::vec3 diffuse;
    float specular;
    uint32_t id;
    glm::ivec3 _pad;
};
