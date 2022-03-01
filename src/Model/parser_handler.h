#pragma once

#include "model.h"
#include <memory>


// Цепочка обязанностей (Chain of Responsibility)
class ParserHandler
{
protected:
    std::shared_ptr<ParserHandler> next;

public:
    virtual ~ParserHandler() = default;

    virtual bool handle(Model& m, const char* line) = 0;

    ParserHandler* add(ParserHandler* p)
    {
        if (next) next->add(p);
        else next = std::shared_ptr<ParserHandler>(p);
        return this;
    }
};


class VertexHandler : public ParserHandler
{
private:
    // const char* key[3] = { "v ", "vn", "vt" };
    const char* format = "%f %f %f";
    glm::vec3 v;

public:
    bool handle(Model& m, const char* line) override
    {
        if (line[0] != 'v' ||
                (line[1] != ' ' && line[1] != 'n' && line[1] != 't'))
            return next ? next->handle(m, line) : false;

        if (sscanf(line + 2, format, &v, &v.y, &v.z) == 3)
        {
            switch (line[1]) {
            case ' ': m.addVertex(v); break;
            // case 'n': m.normal.push_back(v); break;
            // case 't': m.texture.push_back(v); break;
            }
        }

        return true;
    }
};


class Face3Handler : public ParserHandler
{
private:
    const char* key = "f ";
    const char* format[3] = {
        "%d %d %d",
        "%d//%*f %d//%*f %d//%*f",
        "%d/%*f/%*f %d/%*f/%*f %d/%*f/%*f",
    };

    glm::ivec3 v;

public:
    bool handle(Model& m, const char* line) override
    {
        if (line[0] != key[0] || line[1] != key[1])
            return next ? next->handle(m, line) : false;

        for (int i = 0; i < sizeof(format); i++)
        {
            if (sscanf(line + 2, format[i], &v, &v.y, &v.z) == 3)
            {
                m.addFace(v);
                return true;
            }
        }

        return true;
    }
};
