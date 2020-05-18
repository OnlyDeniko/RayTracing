#version 430

#define EPS 1e-3;
#define INF 1e6;

struct Camera{
    vec3 position;
    vec3 view;
    vec3 up;
    vec3 side;
};

struct Ray{
    vec3 origin;
    vec3 direction;
};

struct Sphere{
    vec3 center;
    float radius;
    vec3 color;
    int material_ind;
};

struct Material{
    vec4 light_coeffs;
};

struct Intersection{
    float time;
    vec3 point;
    vec3 normal;
    vec3 color;
    vec4 light_coeffs;
    int material_ind;
};

in vec3 interpolated_vertex;
out vec4 FragColor;

uniform Camera camera;
uniform vec2 scale;
uniform vec3 light_pos;

layout(std430, binding = 0) buffer SphereBuffer{
    Sphere sphere_data[];
};

Material material = Material(vec4(0.4, 0.9, 0.0, 512.0));
//vec3 light_pos = vec3(10, -5, -5);

Ray GenerateRay(Camera camera){
    vec2 coords = interpolated_vertex.xy * normalize(scale);
    vec3 direction = camera.view + camera.side * coords.x + camera.up * coords.y;
    return Ray(camera.position, normalize(direction));
}

bool IntersectSphere(Sphere sphere, Ray ray, out float time){
    ray.origin -= sphere.center;
    float A = dot(ray.direction, ray.direction);
    float B = dot(ray.direction, ray.origin);
    float C = dot(ray.origin, ray.origin) - sphere.radius * sphere.radius;

    float D = B * B - A * C;
    if (D > 0){
        D = sqrt(D);
        float t1 = (-B - D) / (A);
        float t2 = (-B + D) / (A);
        float mi = min(t1, t2);
        float ma = max(t1, t2);

        if (ma < 0) return false;
        if (mi < 0){
            time = ma;
            return true;
        }
        time = mi;
        return true;
    }
    return false;
}

bool Intersect ( Ray ray, float start, float final, inout Intersection intersect ) {
    bool result = false;
    float time = start;
    intersect.time = final;

    for(int i=0; i< 3;i++){
        if( IntersectSphere(sphere_data[i], ray, time) && time < intersect.time ) {
            intersect.time = time;
            intersect.point = ray.origin + ray.direction * time;
            intersect.normal = normalize ( intersect.point - sphere_data[i].center );
            intersect.color = sphere_data[i].color;
            intersect.material_ind = sphere_data[i].material_ind;
            intersect.light_coeffs = material.light_coeffs;

            result = true;
        }
    }
    return result;
}

float Shadow(vec3 pos_light, Intersection intersect){
    float shad = 1.0;
    vec3 direction = normalize(pos_light - intersect.point);
    float dist_light = distance(pos_light, intersect.point);
    vec3 qwe = direction * EPS;
    Ray shad_ray = Ray(intersect.point + qwe, direction);
    Intersection shad_intersect;
    shad_intersect.time = INF;
    if(Intersect(shad_ray, 0 , dist_light, shad_intersect)){
            shad=0.0;
    }
    return shad;
}

vec3 Phong(Intersection intersect, vec3 pos_light, float shadow){
    vec3 light = normalize(pos_light - intersect.point);
    float diffuse = max(dot(light, intersect.normal), 0.0);
    vec3 view = normalize(camera.position - intersect.point);
    vec3 reflected = reflect(-view, intersect.normal);
    float specular = pow(max(dot(reflected, light), 0.0), intersect.light_coeffs.w);

    return intersect.light_coeffs.x * intersect.color +
            intersect.light_coeffs.y * diffuse * intersect.color * shadow +
            intersect.light_coeffs.z * specular;
}

vec4 RayTrace(Ray primary_ray){
    vec4 resColor = vec4(0, 0, 0, 0);
    Ray ray = primary_ray;

    Intersection intersect;
    intersect.time = INF;
    float start = 0;
    float final = INF;

    if (Intersect(ray, start, final, intersect)){
        float shadowing = Shadow(light_pos, intersect);
        resColor += vec4(Phong(intersect, light_pos, shadowing), 0);
    }
    return resColor;
}

void main(void){
    //FragColor = vec4(abs(interpolated_vertex.xy), 0, 1.0);
    Ray ray = GenerateRay(camera);
    FragColor = RayTrace(ray);
    //FragColor = vec4(abs(ray.direction.xy), 0, 1.0);
}
