#include "constant.glsl"

struct Ray
{
    vec3 o;
    vec3 d;
};

struct Camera
{
    vec3 pos;
    vec3 lookat;
    float fov;
};


struct Material
{
    vec3 baseColor;
    float metallic;
    float specular;
    float roughness;
    float trans;        // transmission
    float transRough;   // transmission roughness
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
    // Material material;
    vec3 v0;
    // int _pad0;
    vec3 v1;
    // int _pad1;
    vec3 v2;
    // int _pad2;
};

// struct Sphere
// {
//     vec3 pos;
//     float radius;
//     vec3 diffuse;
//     float specular;
//     int id;
// };

// struct Plane
// {
//     vec3 normal;
//     float distance;
//     vec3 diffuse;
//     float specular;
//     int id;
// };


layout (binding = 1) uniform UBO
{
    vec3 lightPos;
    float aspectRatio;
    Camera camera;
    // mat4 rotMat;
} ubo;

layout (std140, binding = 2) buffer Spheres
{
    Sphere spheres[ ];
};

layout (std140, binding = 3) buffer Planes
{
    Plane planes[ ];
};

layout (std430, binding = 4) buffer Triangles
{
    Triangle triangles[ ];
};

vec3 sphereNormal(in vec3 pos, in Sphere sphere)
{
    return (pos - sphere.pos) / sphere.radius;
}

vec3 triangleNormal(in Ray r, in Triangle tr)
{
    vec3 v1v0 = tr.v1 - tr.v0;
    vec3 v2v0 = tr.v2 - tr.v0;
    vec3 rov0 = r.o - tr.v0;
    vec3  n = cross(v1v0, v2v0);
    return n;
}


// struct Path
// {
//     Ray ray;
//     vec3 li;
//     float alpha;
//     vec3 beta;
//     bool specularBounce;
//     bool abort;
//     float misWeight;
// };

