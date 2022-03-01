#pragma once

#include "entity.h"
#include <vector>


class Model
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::ivec3> faces;

public:
    void addVertex(const glm::vec3 v) { vertices.push_back(v); }
    void addFace(const glm::ivec3 f) { faces.push_back(f); }
    void toTriangles(std::vector<Triangle> &t)
    {
        for (auto& i : faces)
        {
            t.push_back({vertices[i[0]-1], 0, vertices[i[1]-1], 0, vertices[i[2]-1]});
        }
    }
};
