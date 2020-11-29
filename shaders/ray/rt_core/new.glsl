#include "intersect.glsl"
#include "utils.glsl"


struct Path {
    Material material;
    vec3 normal;
    float t;
    vec3 pos;
};


float D_GGX(float NoH, float a) {
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

vec3 F_Schlick(float u, vec3 f0) {
    return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / (GGXV + GGXL);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

vec3 BRDF(vec3 n, vec3 l, Material m) {
    if (dot(n, l) < 0) return vec3(0);

    vec3 v = normalize(ubo.camera.pos);

    vec3 h = normalize(v + l);

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);

    float roughness = m.roughness;
    float a = roughness;
    vec3 f0 = m.baseColor * 0.9;
    vec3 diffuseColor = m.baseColor;

    float D = D_GGX(NoH, a);
    vec3  F = F_Schlick(LoH, f0);
    float V = V_SmithGGXCorrelated(NoV, NoL, roughness);

    vec3 Fr = (D * V) * F;

    vec3 Fd = diffuseColor * Fd_Lambert();

    return Fr + Fd;
}

int getIntersectSurface(Ray r, inout Path p)
{
    float t = MAX_LEN;
    int id = intersect(r, t);
    p.pos = r.o + t * r.d;

    if (id < 0) return id;

    if (id < ubo.n)
    {
        p.normal = sphereNormal(p.pos, spheres[id]);
        p.material = spheres[id].material;
    }
    else if ((id -= ubo.n) < planes.length())
    {
        p.normal = planes[id].normal;
        p.material = planes[id].material;
    }

    return id;
}


vec3 oneRay(Ray r, Path path)
{
    Path tmp_path;
    int id = getIntersectSurface(r, tmp_path);

    vec3 color = vec3(0);
    float income = 0;
    vec3 lightVec;

    if (id == -2) {
        lightVec = ubo.lightPos - path.pos;
        float d = pow(length(lightVec), 2);
        income = clamp(dot(path.normal, normalize(lightVec)) / d, 0, 1);
    }
    else if (id >= 0) {
        lightVec = ubo.lightPos - tmp_path.pos;

        Ray rtmp;
        rtmp.o = tmp_path.pos;
        rtmp.d = normalize(lightVec);
        vec3 rVec;
        float t = MAX_LEN;
        if (intersect(rtmp, t) == -2) {
            float d = pow(length(lightVec), 2);
            // rVec = tmp_path.pos - path.pos;
            // float d2 = pow(length(rVec), 2);
            // if (d2 > 0.1)
                income = clamp(dot(tmp_path.normal, normalize(lightVec)) / PI / d, 0, 1);
        }
    }

    color = BRDF(path.normal, rVec, path.material) * income;
    

    return color;
}

vec3 getColorNew(Ray r)
{
    Path path;
    int id = getIntersectSurface(r, path);

    if (id == -2) return vec3(1);
    if (id == -1) return vec3(0);

    vec3 color = path.material.baseColor / 10;
    vec3 lightVec = normalize(ubo.lightPos - path.pos);

    r.o = path.pos;
    r.d = lightVec;

    color += oneRay(r, path);

    r.d += 2 * -dot(path.normal, r.d) * path.normal;

    color += oneRay(r, path);

    // for (int i = 0; i < 4; i++)
    // {
    //     color += oneRay(r, path);
    // }

    // color /= 2;
    // color *= 8;

    return color;
}
