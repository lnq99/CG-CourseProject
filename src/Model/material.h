#pragma one

#include <glm/vec3.hpp>

// base on principled bdpf
struct Material
{
    glm::vec3 baseColor;    // TODO: texture
    float metallic;
    float specular;
    float roughness;
    float transmission;
} __attribute__((packed));