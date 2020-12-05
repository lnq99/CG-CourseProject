#pragma one

#include "entity.h"

#include <vector>


struct SceneUBO
{
    glm::vec3 lightPos;
    float ambient = 0.2;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 4.0f);
    float aspectRatio;
    glm::mat4 rot;
};


class Scene
{
public:
    std::vector<Plane> planes;
    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
    SceneUBO ubo;
    uint32_t selected = -1;

public:
    static Scene& instance();
    void addSphere(glm::vec3 pos, float radius, Material = { glm::vec3(1), 0, 0.1, 0.5, 0, 0 });
    void addPlane(glm::vec3 normal, float distance, Material = { glm::vec3(1), 0, 0, 0.5, 0, 0 });
    void addTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material = { glm::vec3(1), 0, 0, 0.5, 0, 0 });

private:
    Scene();
    Scene(const Scene&) = delete;
    void operator=(const Scene&) = delete;
};
