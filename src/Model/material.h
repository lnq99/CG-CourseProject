#pragma one

#include <unordered_map>
#include <glm/vec3.hpp>


struct Material
{
    glm::vec3 ambient;
    float ka;
    glm::vec3 diffuse;
    float kd;
    glm::vec3 specular;
    float ks;
    float shininess = 2;
    float reflect = 0.2;
    float _p1;
    float _p2;
};

struct Surface
{
    float ka;
    float kd;
    float ks;
    float shininess;
    float reflect;
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
        add("plastic", { .1, .7, 0, 2, .2 });
    }

    void add(std::string name, Surface sur)
    {
        data.emplace(name, sur);
    }

    Surface getSurface(std::string name)
    {
        return data[name];
    }

    Material get(std::string name, glm::vec3 a, glm::vec3 d, glm::vec3 s)
    {
        Surface sur = getSurface(name);
        return { a, sur.ka, d, sur.kd, s, sur.ks, sur.shininess, sur.reflect };
    }

    Material get(std::string name, glm::vec3 color = glm::vec3(1))
    {
        return get(name, color, color, color);
    }
};
