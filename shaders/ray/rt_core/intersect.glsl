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

float triangleIntersect(in Ray r, in Triangle tr)
{
    vec3 v1v0 = tr.v1 - tr.v0;
    vec3 v2v0 = tr.v2 - tr.v0;
    vec3 rov0 = r.o - tr.v0;

    // Cramer's rule for solcing p(t) = ro+t·rd = p(u,v) = vo + u·(v1-v0) + v·(v2-v1)
    // float d = 1.0/determinant(mat3(v1v0, v2v0, -r.d ));
    // float u =   d*determinant(mat3(rov0, v2v0, -r.d ));
    // float v =   d*determinant(mat3(v1v0, rov0, -r.d ));
    // float t =   d*determinant(mat3(v1v0, v2v0, rov0));

    vec3  n = cross(v1v0, v2v0);
    vec3  q = cross(rov0, r.d);
    float d = 1.0 / dot(r.d, n);
    float u = d * dot(-q, v2v0);
    float v = d * dot( q, v1v0);
    float t = d * dot(-n, rov0);

    if (u < 0.0 || v < 0.0 || (u+v) > 1.0)
        t = -1.0;

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