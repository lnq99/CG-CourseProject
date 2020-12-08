#include "app.h"
#include "entrypoint.h"


static void setupCamera(Camera &c)
{
    c.rotationSpeed = 0.1;
    c.updatePosition();
}

static void createScene(Scene &s)
{
    // Spheres
    s.addSphere({2.5, -0.5, 0}, 0.6, s.store.get("minor", {0.3, 0.6, 0.9}));
    s.addSphere({0.8, -1, 0.2}, 0.5, s.store.get("shiny", {0.6, 0.7, 1}));
    s.addSphere({-1.75, -0.75, -0.8}, 1, s.store.get("metal", {0.9, 0.76, 0.46}));
    s.addSphere({-0.5, -1.5, 0.5}, 0.6, s.store.get("glass", {1, 1, 1}));

    // Planes
    const float halfEdge = 4;
    s.addPlane({0, 1, 0}, halfEdge);
    s.addPlane({0, -1, 0}, halfEdge, s.store.get("diffuse"));
    s.addPlane({0, 0, 1}, halfEdge, s.store.get("minor"));
    s.addPlane({0, 0, -1}, halfEdge);
    s.addPlane({-1, 0, 0}, halfEdge, s.store.get("diffuse", {1, 0, 0}));
    s.addPlane({1, 0, 0}, halfEdge, s.store.get("diffuse", {0, 1, 0}));

    // Boxes
    s.addBox({1.8,-4,-1.2}, {3,-2.5,-0.2}, s.store.get("plastic"));

    // Model
    s.ubo.material = s.store.get("diamond", {1, 1, 1});
    s.addModel("data/diamond.obj");
}

VulkanApp App(setupCamera, createScene);

VULKAN_MAIN()
