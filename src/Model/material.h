#pragma one

#include <unordered_map>
#include <glm/vec3.hpp>


struct Material
{
    glm::vec3 color;
    float ka;
    float kd;
    float ks;
    float shininess = 2;
    float k = 0.2;
    float ior = 0;
    glm::vec3 _p;
};

struct Surface
{
    float ka;
    float kd;
    float ks;
    float shininess;
    float k;
    float ior;
};


class MaterialStore
{
private:
    std::unordered_map<std::string, Surface> data;
public:
    MaterialStore()
    {
        add("metal", { .1, .1, 1, 5, .5 });
        add("shiny", { .1, .6, 2, 5, 0 });
        add("diffuse", { .1, .7, 0, 1, 0 });
        add("minor", { 0, 0, 0, 0, .95 });
        add("plastic", { .1, .7, 0, 2, .1 });
        add("glass", { .2, .2, .2, 2, .8, 1.52 });
        add("diamond", { .1, .1, .1, 0, .9, 2.418 });
    }

    void add(std::string name, Surface sur)
    {
        data.emplace(name, sur);
    }

    Surface getSurface(std::string name)
    {
        return data[name];
    }

    Material get(std::string name, glm::vec3 color = glm::vec3(1))
    {
        Surface sur = getSurface(name);
        return { color, sur.ka, sur.kd, sur.ks, sur.shininess, sur.k, sur.ior };
    }
};
