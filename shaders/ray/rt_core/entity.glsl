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


layout (binding = 1) uniform UBO
{
    vec3 lightPos;
    float ambient;
    vec3 pos;
    float aspectRatio;
    mat4 rotMat;
    Material material;
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
