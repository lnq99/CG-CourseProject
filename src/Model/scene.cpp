#include "scene.h"

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
    spheres.push_back(Sphere { mat, pos, id++, radius });
}

void Scene::addPlane(glm::vec3 normal, float distance, Material mat)
{
    planes.push_back(Plane { mat, normal, id++, distance });
}
