#include "entity.glsl"

float sphereIntersect(in Ray r, in Sphere s)
{
    vec3 oc = r.o - s.pos;
    float b = 2. * dot(oc, r.d);
    float c = dot(oc, oc) - s.radius * s.radius;
    float h = b * b - 4 * c;

    if (h < 0) return -1;

    float t = (-b - sqrt(h)) / 2;

    return t;
}

float planeIntersect(in Ray r, in Plane p)
{
    float d = dot(r.d, p.normal);

    if (d == 0) return -1;

    float t = -(p.distance + dot(r.o, p.normal)) / d;

    return t;
}

float triangleIntersect(in Ray r, in Triangle tr)
{
    vec3 v1v0 = tr.v1 - tr.v0;
    vec3 v2v0 = tr.v2 - tr.v0;
    vec3 rov0 = r.o - tr.v0;

    vec3  n = cross(v1v0, v2v0);
    vec3  q = cross(rov0, r.d);
    float d = 1 / dot(r.d, n);
    float u = d * dot(-q, v2v0);
    float v = d * dot( q, v1v0);
    float t = d * dot(-n, rov0);

    if (u < 0 || v < 0 || (u+v) > 1)
        t = -1;

    return t;
}

int intersect(in Ray r, out float tRest)
{
    int id = -1;

    float tMin = MAX_LEN;
    float t;

    Material m;

    Sphere light = Sphere(
        m,
        ubo.lightPos,
        0.05
    );

    t = sphereIntersect(r, light);

    if (t != -1)
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

    for (int i = 0; i < triangles.length(); i++)
    {
        t = triangleIntersect(r, triangles[i]);

        if (t > EPSILON && t < tMin)
        {
            id = i + spheres.length() + planes.length();
            tMin = t;
        }
    }

    tRest = tMin;

    return id;
}

// Shadow ray
int intersect(in Ray r)
{
    float t;
    Material m;

    Sphere light = Sphere(
        m,
        ubo.lightPos,
        0.05
    );

    float tMin = sphereIntersect(r, light);
    int id = -2;

    for (int i = 0; i < spheres.length(); i++)
    {
        t = sphereIntersect(r, spheres[i]);

        if (t > EPSILON && t < tMin)
            return 0;
    }

    for (int i = 0; i < planes.length(); i++)
    {
        t = planeIntersect(r, planes[i]);

        if (t > EPSILON && t < tMin)
            return 0;
    }

    for (int i = 0; i < triangles.length(); i++)
    {
        t = triangleIntersect(r, triangles[i]);

        if (t > EPSILON && t < tMin)
            return 0;
    }

    return id;
}