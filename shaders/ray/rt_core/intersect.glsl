#include "entity.glsl"

float sphereIntersect(in Ray r, in Sphere s)
{
    vec3 oc = r.o - s.pos;
    float b = 2. * dot(oc, r.d);
    float c = dot(oc, oc) - s.radius * s.radius;
    float h = b * b - 4 * c;
    if (h < 0)
        return -1;
    float t = (-b - sqrt(h)) / 2;
    return t;
}

float planeIntersect(in Ray r, in Plane p)
{
    float d = dot(r.d, p.normal);

    if (d == 0) return 0;

    float t = -(p.distance + dot(r.o, p.normal)) / d;

    if (t < 0) return 0;

    return t;
}

int intersect(in Ray r, inout float tRest)
// int intersect(in Ray r, inout Path p)
{
    int id = -1;

    float tMin = tRest;
    float t;

    Material m;

    Sphere light = Sphere(
        m,
        ubo.lightPos,
        -2,
        0.05
    );

    t = sphereIntersect(r, light);

    if (t > EPSILON && t < tMin)
    {
        id = -2;
        tMin = t;
    }

    for (int i = 0; i < spheres.length(); i++)
    {
        t = sphereIntersect(r, spheres[i]);

        if (t > EPSILON && t < tMin)
        {
            id = i;
            tMin = t;
        }
    }

    for (int i = 0; i < planes.length(); i++)
    {
        t = planeIntersect(r, planes[i]);

        if (t > EPSILON && t < tMin)
        {
            id = i + spheres.length();
            tMin = t;
        }
    }

    tRest = tMin;

    return id;
}