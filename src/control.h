#pragma one

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Controller
{
    glm::vec3 translate;
    glm::vec3 rotate;
    glm::vec3 scale;

    glm::vec4 color;

    int isMetal = 0;
};
