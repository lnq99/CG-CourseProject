#include "intersect.glsl"


float lightDiffuse(vec3 normal, vec3 lightDir)
{
    return max(dot(normal, lightDir), 0);
}

float lightSpecular(vec3 normal, vec3 lightDir, float shininess)
{
    vec3 viewVec = normalize(ubo.pos);
    vec3 halfVec = normalize(lightDir + viewVec);
    return pow(max(dot(normal, halfVec), 0), shininess);
}


void calcDiffuseSpecular(inout vec2 df, vec3 normal, vec3 pos, vec3 lightVec, float shininess)
{
    Ray shadowRay = Ray(pos, lightVec);

    if (intersect(shadowRay) == -2)
    {
        df[0] = lightDiffuse(normal, lightVec);
        df[1] = lightSpecular(normal, lightVec, shininess);
    }
}

vec3 calcColor(vec2 df, Material m)
{
    return ubo.ambient * m.ambient * m.ka
            + df[0] * m.diffuse * m.kd
            + df[1] * m.specular * m.ks;
}


vec3 getColor(inout Ray r, inout float k, inout float ior)
{
    float t;
    uint id = intersect(r, t);

    vec3 color = vec3(0);
    vec3 normal;
    vec3 pos = r.o + t * r.d;
    vec3 lightVec = normalize(ubo.lightPos - pos);

    vec2 df = vec2(0);
    Material m;

    if (id == -2) return vec3(1);

    // if (id == -1) return vec3(0);

    if (id < spheres.length())
    {
        normal = sphereNormal(pos, spheres[id]);
        m = spheres[id].material;
    }
    else if ((id -= spheres.length()) < planes.length())
    {
        normal = planes[id].normal;
        m = planes[id].material;
    }
    else if ((id -= planes.length()) < triangles.length())
    {
        normal = triangleNormal(triangles[id]);
        m = ubo.material;
    }

    if (m.ior != 0)
    {
        r.o = pos;
        vec3 d;
        if (dot(r.d, normal) > 0) normal = -normal;
        if (m.ior == ior) m.ior = 1;
        d = refract(r.d, normal, ior / m.ior);

        if (length(d) != 0)
        {
            r.d = d;
            k *= 0.98;
        }
        else
        {
            r.d = reflect(r.d, normal);
            k *= 0.9;
        }
        ior = m.ior;
        return color;
    }

    calcDiffuseSpecular(df, normal, pos, lightVec, m.shininess);

    color = calcColor(df, m);

    k *= m.k;

    r.o = pos;
    r.d = reflect(r.d, normal);

    return color;
}
