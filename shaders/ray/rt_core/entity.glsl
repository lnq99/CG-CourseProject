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
    int id;
    // vec3 _pad;
};

struct Plane
{
    Material material;
    vec3 normal;
    int id;
    float distance;
    // vec3 _pad;
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
    vec4 fogColor;
    Camera camera;
    mat4 rotMat;
    int n;
} ubo;

layout (std140, binding = 2) buffer Spheres
{
    Sphere spheres[ ];
};

layout (std140, binding = 3) buffer Planes
{
    Plane planes[ ];
};


vec3 sphereNormal(in vec3 pos, in Sphere sphere)
{
    return (pos - sphere.pos) / sphere.radius;
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

