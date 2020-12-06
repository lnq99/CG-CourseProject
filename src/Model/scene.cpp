#include "scene.h"

const Material Scene::defaultMaterial = { glm::vec3(1), .1, .7, 0 };


Scene::Scene()
{
}

Scene& Scene::instance()
{
    static Scene scene;
    return scene;
}

void Scene::addSphere(glm::vec3 pos, float radius, Material mat)
{
    spheres.push_back(Sphere { mat, pos, radius });
}

void Scene::addPlane(glm::vec3 normal, float distance, Material mat)
{
    planes.push_back(Plane { mat, normal, distance });
}

void Scene::addTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material mat)
{
    triangles.push_back(Triangle { v0, 0, v1, 0, v2 });
}