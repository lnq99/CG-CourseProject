#pragma one

#include <glm/vec3.hpp>

// base on principled bsdf
struct Material
{
    glm::vec3 baseColor;    // TODO: texture
    float metallic;
    float specular;
    float roughness;
    float trans;
    float transRough;
};
