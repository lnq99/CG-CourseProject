float rand(vec3 scale, float seed) {
    // use the fragment position for a different seed per-pixel
    return fract(sin(dot(gl_GlobalInvocationID.xyz + seed, scale)) * 43758.5453 + seed);
}

float rand(vec3 scale) {
    return fract(sin(dot(gl_GlobalInvocationID.xyz + 2.345, scale)) * 43758.5453 + 2.345);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}
