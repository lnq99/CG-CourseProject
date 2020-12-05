#include "intersect.glsl"


float lightDiffuse(vec3 normal, vec3 lightDir)
{
    return clamp(dot(normal, lightDir), 0, 1);
}

float lightSpecular(vec3 normal, vec3 lightDir, float specularFactor)
{
    vec3 viewVec = normalize(ubo.pos);
    vec3 halfVec = normalize(lightDir + viewVec);
    return specularFactor * pow(clamp(dot(normal, halfVec), 0, 1), 32);
}


vec3 getColor(inout Ray r)
{
    vec3 color = vec3(0);

    float t = MAX_LEN;

    uint id = intersect(r, t);

    vec3 normal;
    vec3 pos = r.o + t * r.d;
    vec3 lightVec = normalize(ubo.lightPos - pos);

    float diffuse = 0.2, specular = 0;

    if (id == -2) return vec3(2);

    if (id == -1) return vec3(0);

    if (id < spheres.length())
    {
        t = sphereIntersect(r, spheres[id]);

        normal = sphereNormal(pos, spheres[id]);

        Ray rtmp = Ray(pos, lightVec);

        if (intersect(rtmp, t) < 0)
        {
            diffuse = lightDiffuse(normal, lightVec);
            specular = lightSpecular(normal, lightVec, spheres[id].material.specular);
        }

        color = diffuse * spheres[id].material.baseColor + specular;
    }
    else if ((id -= spheres.length()) < planes.length())
    {
        t = planeIntersect(r, planes[id]);

        normal = planes[id].normal;
        Ray rtmp = Ray(pos, lightVec);

        if (intersect(rtmp, t) < 0)
        {
            diffuse = lightDiffuse(normal, lightVec);
            specular = lightSpecular(normal, lightVec, planes[id].material.specular);
        }

        color = diffuse * planes[id].material.baseColor + specular;
    }
    else if ((id -= planes.length()) < triangles.length())
    {
        t = triangleIntersect(r, triangles[id]);

        normal = triangleNormal(triangles[id]);
        Ray rtmp = Ray(pos, lightVec);

        if (intersect(rtmp, t) < 0)
        {
            diffuse = lightDiffuse(normal, lightVec);
            specular = lightSpecular(normal, lightVec, 0.8);
        }

        color = diffuse * vec3(1, 1, 0) + specular;
    }

    r.o = pos;
    r.d += 2 * -dot(normal, r.d) * normal;

    return color;
}
