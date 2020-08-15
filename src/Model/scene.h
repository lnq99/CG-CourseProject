#pragma one

#include "light.h"
// #include "camera.h"
#include "entity.h"

#include <vector>


struct SceneUBO
{
    glm::vec3 lightPos;
    float aspectRatio;
    glm::vec4 fogColor = glm::vec4(0.0f);
    struct {
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 4.0f);
        glm::vec3 lookat = glm::vec3(0.0f, 0.5f, 0.0f);
        float fov = 10.0f;
    } camera;
    uint pad;
    uint n;
};


class Scene
{
public:
    std::vector<Plane> planes;
    std::vector<Sphere> spheres;
    SceneUBO ubo;
    uint32_t selected;

public:
    static Scene& instance();
    void unSelect();
    void addSphere(glm::vec3 pos, float radius, glm::vec3 diffuse, float specular);
    void addPlane(glm::vec3 normal, float distance, glm::vec3 diffuse, float specular);

private:
    Scene();
    Scene(const Scene&) = delete;
    void operator=(const Scene&) = delete;
    uint32_t id = 0;
};


