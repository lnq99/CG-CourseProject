#pragma one

#include "entity.h"

#include <vector>


struct SceneUBO
{
    glm::vec3 lightPos;
    float ambient = 1;
    glm::vec3 cameraPos;
    float aspectRatio;
    glm::mat4 rot;
    Material material;
    glm::vec3 lightColor;
    float lightRadius;
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
    void addSphere(glm::vec3 pos, float radius, Material = defaultMaterial);
    void addPlane(glm::vec3 normal, float distance, Material = defaultMaterial);
    void addTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material = defaultMaterial);

private:
    Scene();
    Scene(const Scene&) = delete;
    void operator=(const Scene&) = delete;
    static const Material defaultMaterial;
};
