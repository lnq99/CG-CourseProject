#include "constant.glsl"

struct Ray
{
    vec3 o;
    vec3 d;
};

struct Material
{
    vec3 color;
    float ka;
    float kd;
    float ks;
    float shininess;
    float k;
    float ior;
};

struct Sphere
{
    Material material;
    vec3 pos;
    float radius;
};

struct Plane
{
    Material material;
    vec3 normal;
    float distance;
};

struct Triangle
{
    vec3 v0;
    vec3 v1;
    vec3 v2;
};

// Axis Aligned Bounding Box
struct AABB
{
    Material material;
    vec3 bMin;
    vec3 bMax;
};


layout (binding = 1) uniform UBO
{
    vec3 lightPos;
    float ambient;
    vec3 cameraPos;
    float aspectRatio;
    mat4 rotMat;
    Material material;
    vec3 lightColor;
    float lightRadius;
} ubo;

layout (std140, binding = 2) buffer Spheres
{
    Sphere spheres[];
};

layout (std140, binding = 3) buffer Planes
{
    Plane planes[];
};

layout (std430, binding = 4) buffer Triangles
{
    Triangle triangles[];
};

layout (std430, binding = 5) buffer AABBs
{
    AABB boxes[];
};


vec3 sphereNormal(in vec3 pos, in Sphere sphere)
{
    return (pos - sphere.pos) / sphere.radius;
}

vec3 triangleNormal(in Triangle tr)
{
    vec3 v1v0 = tr.v1 - tr.v0;
    vec3 v2v0 = tr.v2 - tr.v0;
    vec3  n = normalize(cross(v1v0, v2v0));
    return n;
}

vec3 aabbNormal(in vec3 pos, in AABB box)
{
    vec3 n = vec3(0);
    if (abs(pos.z - box.bMax.z) < EPSILON)
        n.z = 1;
    else if (abs(pos.y - box.bMax.y) < EPSILON)
        n.y = 1;
    else if (abs(pos.x - box.bMax.x) < EPSILON)
        n.x = 1;
    else if (abs(pos.x - box.bMin.x) < EPSILON)
        n.x = -1;
    else if (abs(pos.y - box.bMin.y) < EPSILON)
        n.y = -1;
    else
        n.z = -1;
    return n;
}
