#include "intersect.glsl"
#include "bsdf.glsl"


float lightDiffuse(vec3 normal, vec3 lightDir)
{
    return clamp(dot(normal, lightDir), 0.1, 1);
    return clamp(dot(normal, lightDir), 0.1, 1) / pow(length(lightDir), 2);
}

vec3 lightDiffuse2(vec3 normal, vec3 lightDir, Material m)
{
    vec3 viewDir = normalize(ubo.camera.pos);
    vec3 halfVector = normalize(viewDir+lightDir);

    // cosThetaL = abs(cosThetaL);
    float cosThetaL = dot(lightDir, normal);
    float diffuseWeight = 0.9;

    float cosThetaV = abs(dot(normal,viewDir));
    float cosThetaH = abs(dot(normal,halfVector));
    float cosThetaD = abs(dot(lightDir,halfVector));

    float alpha2 = (m.roughness*m.roughness)*(m.roughness*m.roughness);

    float F = fresnelSchlick(cosThetaD,mix(R0,.6,m.metallic));
    float D = trowbridgeReitzD(cosThetaH,alpha2);

    float roughnessRemapped = .5+.5*m.roughness;
    float alpha2Remapped = (roughnessRemapped*roughnessRemapped)*(roughnessRemapped*roughnessRemapped);

    float G = 1./(1.+trowbridgeReitzLambda(cosThetaV,alpha2Remapped)+trowbridgeReitzLambda(cosThetaL,alpha2Remapped));

    float specular = F*D*G/(4.*cosThetaV*cosThetaL);
    float specularPdf = D*cosThetaH/(4.*cosThetaD);

    float f = -.5+2.*cosThetaD*cosThetaD*m.roughness;
    float diffuse = diffuseWeight*INVPI*(1.+f*fresnelSchlickWeight(cosThetaL))*(1.+f*fresnelSchlickWeight(cosThetaV));
    float diffusePdf = cosThetaL*INVPI;

    // pdf = mix(.5*(specularPdf+diffusePdf),specularPdf,m.metallic);

    return mix(m.baseColor*diffuse+specular,m.baseColor*specular,m.metallic);
}

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

vec3 BRDF(vec3 n, vec3 l, Material m, Ray r) {
    vec3 v = normalize(ubo.camera.pos);

    vec3 h = normalize(v + l);

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);

    // perceptually linear roughness to roughness (see parameterization)
    // float roughness = perceptualRoughness * perceptualRoughness;
    float roughness = m.roughness;
    float a = roughness;
    vec3 f0 = m.baseColor * 0.9;
    vec3 diffuseColor = m.baseColor;

    float D = D_GGX(NoH, a);
    vec3  F = F_Schlick(LoH, f0);
    float V = V_SmithGGXCorrelated(NoV, NoL, roughness);

    // specular BRDF
    vec3 Fr = (D * V) * F;

    // diffuse BRDF
    vec3 Fd = diffuseColor * Fd_Lambert();

    // apply lighting...

    if (dot(n, l) < 0)
    {
        // return (Fr + Fd) * 0.8;
        return vec3(0);
    }

    // vec3 colorR = Fr * 

    return Fr + Fd;
}

float lightSpecular(vec3 normal, vec3 lightDir, float specularFactor)
{
    vec3 viewVec = normalize(ubo.camera.pos);
    vec3 halfVec = normalize(lightDir + viewVec);
    return specularFactor * pow(clamp(dot(normal, halfVec), 0, 1), 32);
}

vec3 getColor(inout Ray r)
{
    vec3 color = vec3(0);
    // color = vec3((r.d.y + 1) / 2, (r.d.y + 1) / 2, 1);

    float t = MAX_LEN;

    uint id = intersect(r, t);

    vec3 normal;
    vec3 pos = r.o + t * r.d;
    vec3 lightVec = normalize(ubo.lightPos - pos);

    float diffuse = 0.1, specular = 0;

    if (id == -2) return vec3(2);

    if (id == -1) return vec3(0);

    if (id < ubo.n)
    {
        t = sphereIntersect(r, spheres[id]);

        // r.o = pos;
        // r.d += 2 * -dot(normal, r.d) * normal;

        normal = sphereNormal(pos, spheres[id]);

        // if (dot(normal, lightVec) < 0)  return vec3(0);

        Ray rtmp = Ray(pos, lightVec);

        if (intersect(rtmp, t) < 0)
        {
            // color = lightDiffuse2(normal, lightVec, spheres[id].material);
            // color = BRDF(normal, lightVec, spheres[id].material, r);
            diffuse = lightDiffuse(normal, lightVec);
            specular = lightSpecular(normal, lightVec, spheres[id].material.specular);
        }

        color = diffuse * spheres[id].material.baseColor + specular;
    }
    else if ((id -= ubo.n) < planes.length())
    {
        t = planeIntersect(r, planes[id]);

        // r.o = pos;
        // r.d += 2 * -dot(normal, r.d) * normal;

        normal = planes[id].normal;
        Ray rtmp = Ray(pos, lightVec);

        if (intersect(rtmp, t) < 0)
        {
            // color = lightDiffuse2(normal, lightVec, planes[id].material);
            // color = BRDF(normal, lightVec, planes[id].material, r);
            diffuse = lightDiffuse(normal, lightVec);
            specular = lightSpecular(normal, lightVec, planes[id].material.specular);
        }

        color = diffuse * planes[id].material.baseColor + specular;
    }
    // else
    // {
        r.o = pos;
        r.d += 2 * -dot(normal, r.d) * normal;
    // }

    return color;
}

