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

float aabbIntersect(in Ray r, in AABB box) {
    vec3 tbot = (box.bMin - r.o) / r.d;
    vec3 ttop = (box.bMax - r.o) / r.d;
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    float t1 = max(max(tmin.x, tmin.y), tmin.z);
    float t2 = min(min(tmax.x, tmax.y), tmax.z);
    if (t2 <= t1)
        return -1;
    return t1;
}

// Möller-Trumbore algorithm
// Алгоритм Моллера — Трумбора
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
float triangleIntersect(in Ray r, in Triangle tr)
{
    vec3 v0v1 = tr.v1 - tr.v0;
    vec3 v0v2 = tr.v2 - tr.v0;
    // vec3 n = cross(v0v1, v0v2);
    // vec3 v0ro = r.o - tr.v0;
    // vec3 q = cross(v0ro, r.d);
    // float d = 1 / dot(r.d, n);
    // float u = d * dot(-q, v0v2);
    // float v = d * dot( q, v0v1);

    // if (u < 0 || v < 0 || (u+v) > 1)
    //     return -1;

    // float t = d * dot(-n, v0ro);
    // return t;

    vec3 pvec = cross(r.d, v0v2);
    float det = dot(v0v1, pvec);
    if (abs(det) < EPSILON) return -1;

    float invDet = 1 / det;
    vec3 tvec = r.o - tr.v0;
    float u = dot(tvec, pvec) * invDet;
    if (u < 0 || u > 1) return -1;

    vec3 qvec = cross(tvec, v0v1);
    float v = dot(r.d, qvec) * invDet;
    if (v < 0 || u + v > 1) return -1;

    float t = dot(v0v2, qvec) * invDet;
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

    int n = 0;

    for (n = 0; n < spheres.length(); n++)
    {
        t = sphereIntersect(r, spheres[n]);

        if (t > EPSILON && t < tMin)
        {
            id = n;
            tMin = t;
        }
    }

    for (int i = 0; i < planes.length(); i++, n++)
    {
        t = planeIntersect(r, planes[i]);

        if (t > EPSILON && t < tMin)
        {
            id = n;
            tMin = t;
        }
    }

    for (int i = 0; i < triangles.length(); i++, n++)
    {
        t = triangleIntersect(r, triangles[i]);

        if (t > EPSILON && t < tMin)
        {
            id = n;
            tMin = t;
        }
    }

    for (int i = 0; i < boxes.length(); i++, n++)
    {
        t = aabbIntersect(r, boxes[i]);

        if (t > EPSILON && t < tMin)
        {
            id = n;
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

    for (int i = 0; i < boxes.length(); i++)
    {
        t = aabbIntersect(r, boxes[i]);

        if (t > EPSILON && t < tMin)
            return 0;
    }

    return id;
}