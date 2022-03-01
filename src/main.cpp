#include "app.h"
#include "entrypoint.h"


static void setupCamera(Camera &c)
{
    c.rotationSpeed = 0.1;
    c.updatePosition();
}

static void createScene(Scene &s)
{
    auto& m = s.store;

    // Spheres
    s.addSphere({2.5,-0.5,0}, 0.6, m.get("minor", {0.3,0.6,0.9}));
    s.addSphere({0.8,-1,0.2}, 0.5, m.get("shiny", {0.6,0.7,1}));
    s.addSphere({-1.7,-0.7,-0.8}, 1, m.get("metal", {0.9,0.8,0.5}));
    s.addSphere({-0.5,-1.5,0.5}, 0.6, m.get("glass", {1,1,1}));

    // Planes
    const float half = 4;
    s.addPlane({0,1,0}, half);
    s.addPlane({0,-1,0}, half, m.get("diffuse"));
    s.addPlane({0,0,1}, half, m.get("minor"));
    s.addPlane({0,0,-1}, half);
    s.addPlane({-1,0,0}, half, m.get("diffuse", {1,0,0}));
    s.addPlane({1,0,0}, half, m.get("diffuse", {0,1,0}));

    // Boxes
    s.addBox({1.8,-4,-1.2},{3,-2.5,-0.2}, m.get("plastic"));

    // Model
    s.ubo.material = m.get("diamond",{1,1,1});
    s.addModel("data/diamond.obj");


    // Benchmark

    // const float half = 4;
    // s.addPlane({0,1,0}, half, m.get("diffuse"));
    // s.addPlane({0,-1,0}, half, m.get("diffuse"));
    // s.addPlane({0,0,1}, half, m.get("diffuse"));
    // s.addPlane({0,0,-1}, half, m.get("diffuse"));
    // s.addPlane({-1,0,0}, half, m.get("diffuse"));
    // s.addPlane({1,0,0}, half, m.get("diffuse"));

    // auto randPoint = [](int lo=-4, int hi=4) 
    // {
    //     return glm::vec3(
    //         lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(hi-lo))),
    //         lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(hi-lo))),
    //         lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(hi-lo)))
    //     );
    // };

    // for (auto i = 0; i < 28; i++)
    // {
    //     s.addTriangle(randPoint(),randPoint(),randPoint());
    // }
}

VulkanApp App(setupCamera, createScene);

VULKAN_MAIN()
