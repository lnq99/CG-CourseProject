#include "app.h"
#include "entrypoint.h"


static void setupCamera(Camera &c)
{
    // c.setPerspective(45.0f, (float)width / (float)height, 0, 256);
    // c.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    c.setTranslation(glm::vec3(0.0f, 0.0f, -4.0f));
    c.rotationSpeed = 0.05f;
    c.movementSpeed = 2.5f;
}

static void createScene(Scene &s)
{
    Material metal1 = { glm::vec3(1), 0, 0.6, 0.5, 0, 0 };
    Material metal2 = { glm::vec3(1), 0, 0.6, 0.5, 0, 0 };
    Material plastic = { glm::vec3(1), 0, 0.6, 0.5, 0, 0 };
    Material glass = { glm::vec3(1), 0, 0.6, 0.5, 0, 0 };
    Material mat = { glm::vec3(1), 0, 0.6, 0.5, 0, 0 };


    // Spheres
    s.addSphere(glm::vec3(2.75, -0.5, 0), 1, {glm::vec3(0, 1, 0), 0.3, 0.6, 0.5});
    s.addSphere(glm::vec3(0, 1, -0.5), 1, {glm::vec3(0.65, 0.77, 0.97f), 0, 0.6, 0.8});
    s.addSphere(glm::vec3(-1.75, -0.75, -0.5f), 1.25, {glm::vec3(0.9, 0.76, 0.46f), 1, 0.6, 0.2});

    // Planes
    const float halfEdge = 4;
    s.addPlane(glm::vec3(0, 1, 0), halfEdge);
    s.addPlane(glm::vec3(0, -1, 0), halfEdge);
    s.addPlane(glm::vec3(0, 0, 1), halfEdge);
    s.addPlane(glm::vec3(0, 0, -1), halfEdge, {glm::vec3(0), 0.6});
    s.addPlane(glm::vec3(-1, 0, 0), halfEdge, {glm::vec3(1, 0, 0), 0.6});
    s.addPlane(glm::vec3(1, 0, 0), halfEdge, {glm::vec3(0, 1, 0), 0.6});

    for (int i = 0; i < 2; i++)
        s.addSphere(
            glm::vec3((rand() % 10 / 5.f - 1) * 4, (rand() % 10 / 5.f - 1) * 3, (rand() % 10 / 5.f - 1) * 1.2),
            (rand() % 7 + 2) / 6.f,
            {glm::vec3((rand() % 10) / 10.f, 0.2f, (rand() % 10) / 10.f),
            (rand() % 10) / 10.f});
}

VulkanApp App(setupCamera, createScene);

VULKAN_MAIN()
