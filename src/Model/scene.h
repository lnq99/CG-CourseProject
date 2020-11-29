#pragma one

#include "light.h"
// #include "camera.h"
#include "entity.h"

#include <vector>


struct SceneUBO
{
    glm::vec3 lightPos;
    float aspectRatio;
    struct {
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 4.0f);
    } camera;
    uint pad;
    // uint n;
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
    void addSphere(glm::vec3 pos, float radius, Material = { glm::vec3(1), 0, 0.5, 0.5, 0, 0 });
    void addPlane(glm::vec3 normal, float distance, Material = { glm::vec3(1), 0, 0.5, 0.5, 0, 0 });

private:
    Scene();
    Scene(const Scene&) = delete;
    void operator=(const Scene&) = delete;
    int32_t id = 0;
};
