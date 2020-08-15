#include "scene.h"

Scene::Scene()
{
}

Scene& Scene::instance()
{
    static Scene scene;
    return scene;
}

void Scene::addSphere(glm::vec3 pos, float radius, glm::vec3 diffuse, float specular)
{
    spheres.push_back(Sphere { pos, radius, diffuse, specular, id++ });
}

void Scene::addPlane(glm::vec3 normal, float distance, glm::vec3 diffuse, float specular)
{
    planes.push_back(Plane { normal, distance, diffuse, specular, id++ });
}
