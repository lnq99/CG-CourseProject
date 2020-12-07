#pragma one
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    float radius = 3.8;
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 position;
    glm::mat4 rotM = glm::mat4(1);
    float rotationSpeed = 0.1;

    void updatePosition()
    {
        position = rotM * glm::vec4(0, 0, -radius, 0);
    }

    void rotate(glm::vec3 delta)
    {
        rotation += delta;
        rotM = glm::mat4(1);
        rotM = glm::rotate(rotM, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        updatePosition();
    }
};
