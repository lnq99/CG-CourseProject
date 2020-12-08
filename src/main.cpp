#include "app.h"
#include "entrypoint.h"


static void setupCamera(Camera &c)
{
    c.rotationSpeed = 0.1;
    c.updatePosition();
}

static void createScene(Scene &s)
{
    s.ubo.material = s.store.get("diamond", {1, 1, 1});

    // Spheres
    s.addSphere(glm::vec3(2.5, -0.5, 0), 0.6, s.store.get("plastic", glm::vec3(0.3, 0.6, 0.9)));
    s.addSphere(glm::vec3(0.8, -1, 0.2), 0.5, s.store.get("shiny", glm::vec3(0.6, 0.7, 1)));
    s.addSphere(glm::vec3(-1.75, -0.75, -0.8), 1, s.store.get("metal", glm::vec3(0.9, 0.76, 0.46)));
    s.addSphere(glm::vec3(-0.5, -1.2, 0.5), 0.6, s.store.get("glass", {1, 1, 1}));

    // Planes
    const float halfEdge = 4;
    s.addPlane(glm::vec3(0, 1, 0), halfEdge);
    s.addPlane(glm::vec3(0, -1, 0), halfEdge, s.store.get("diffuse"));
    s.addPlane(glm::vec3(0, 0, 1), halfEdge, s.store.get("minor"));
    s.addPlane(glm::vec3(0, 0, -1), halfEdge);
    s.addPlane(glm::vec3(-1, 0, 0), halfEdge, s.store.get("diffuse", {1, 0, 0}));
    s.addPlane(glm::vec3(1, 0, 0), halfEdge, s.store.get("diffuse", {0, 1, 0}));


    glm::vec3 v[] = {
        {},
        { 0.000000, 0.500000, 1.000000 },
        { 0.866025, 0.500000, 0.500000 },
        { 0.866025, 0.500000, -0.500000 },
        { -0.000000, 0.500000, -1.000000 },
        { -0.866025, 0.500000, -0.500000 },
        { -0.866025, 0.500000, 0.500000 },
        { 0.000000, -0.500000, -0.000000 },
        { 0.000000, 0.754472, 0.776881 },
        { 0.672799, 0.754472, 0.388440 },
        { 0.672798, 0.754472, -0.388440 },
        { -0.000000, 0.754472, -0.776881 },
        { -0.672799, 0.754472, -0.388440 },
        { -0.672799, 0.754472, 0.388440 },
    };

    std::vector<std::vector<int>> f = {
        { 1, 7, 2 },
        { 2, 7, 3 },
        { 3, 7, 4 },
        { 4, 7, 5 },
        { 6, 8, 13 },
        { 5, 7, 6 },
        { 6, 7, 1 },
        { 9, 11, 13 },
        { 2, 8, 1 },
        { 6, 12, 5 },
        { 5, 11, 4 },
        { 4, 10, 3 },
        { 2, 10, 9 },
        { 6, 1, 8 },
        { 13, 8, 9 },
        { 9, 10, 11 },
        { 11, 12, 13 },
        { 2, 9, 8 },
        { 6, 13, 12 },
        { 5, 12, 11 },
        { 4, 11, 10 },
        { 2, 3, 10 },
    };


    for (auto i : f) {
        s.addTriangle(v[i[0]], v[i[1]], v[i[2]]);
    }

    s.addBox({1.5,-4,-2}, {2.5,-2.5,-1}, s.store.get("minor"));
}

VulkanApp App(setupCamera, createScene);

VULKAN_MAIN()
