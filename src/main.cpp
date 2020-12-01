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
    s.addSphere(glm::vec3(2.75, -0.5, 0), 1, {glm::vec3(0, 1, 0), 0.3, 0, 0.5});
    s.addSphere(glm::vec3(0, 1, -0.5), 1, {glm::vec3(0.65, 0.77, 0.97f), 0, 1, 0.8});
    s.addSphere(glm::vec3(-1.75, -0.75, -0.5f), 1.25, {glm::vec3(0.9, 0.76, 0.46f), 0.5, 0.6, 0.2});

    // Planes
    const float halfEdge = 4;
    s.addPlane(glm::vec3(0, 1, 0), halfEdge);
    s.addPlane(glm::vec3(0, -1, 0), halfEdge);
    s.addPlane(glm::vec3(0, 0, 1), halfEdge);
    s.addPlane(glm::vec3(0, 0, -1), halfEdge, {glm::vec3(0), 0.6});
    s.addPlane(glm::vec3(-1, 0, 0), halfEdge, {glm::vec3(1, 0, 0), 0.6});
    s.addPlane(glm::vec3(1, 0, 0), halfEdge, {glm::vec3(0, 1, 0), 0.6});


    // std::vector<glm::vec3> v = {
    //     {  0,  0,  1.75 },
    //     {  1,  1,  0 },
    //     {  1, -1,  0 },
    //     { -1, -1,  0 },
    //     { -1,  1,  0 },
    //     {  0,  0, -1.75 },
    // };

    // std::vector<std::vector<int>> f = {
    //     { 1, 2, 3 },
    //     { 1, 3, 4 },
    //     { 1, 4, 5 },
    //     { 1, 5, 2 },
    //     { 6, 5, 4 },
    //     { 6, 4, 3 },
    //     { 6, 3, 2 },
    //     { 6, 2, 1 },
    //     { 6, 1, 5 }
    // };


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

    // Triangles
    // s.addTriangle(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0.5), glm::vec3(0, 0, 1), {glm::vec3(0, 1, 1), 0.6});
    // s.addTriangle(glm::vec3(1, 1, 0.2), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), {glm::vec3(0, 1, 1), 0.6});



    // for (int i = 0; i < 2; i++)
    //     s.addSphere(
    //         glm::vec3((rand() % 10 / 5.f - 1) * 4, (rand() % 10 / 5.f - 1) * 3, (rand() % 10 / 5.f - 1) * 1.2),
    //         (rand() % 7 + 2) / 6.f,
    //         {glm::vec3((rand() % 10) / 10.f, 0.2f, (rand() % 10) / 10.f),
    //         (rand() % 10) / 10.f});
}

VulkanApp App(setupCamera, createScene);

VULKAN_MAIN()
