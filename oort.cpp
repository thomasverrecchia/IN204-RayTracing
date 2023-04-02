#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "vectors.hpp"
#include "sphere.hpp"
#include "object.hpp"
#include "light.hpp"



bool scene_intersect(const Vec3f &orig, const Vec3f &dir, const std::vector<Object*> &objects, Vec3f &hit, Vec3f &N, Material &material) {
    float closest_dist = std::numeric_limits<float>::max();
    for (const auto& object : objects) {
        float dist_i;
        if (object->ray_intersect(orig, dir, dist_i) && dist_i < closest_dist) {
            closest_dist = dist_i;
            hit = orig + dir*dist_i;
            N = object->get_normal(hit);
            material = object->get_material();
        }
    }
    return closest_dist < 1000;
}

Vec3f reflect(const Vec3f &I, const Vec3f &N) {
    return I - N*2.f*(I*N);
}

Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const std::vector<Object*> &objects, const std::vector<Light> &lights,
char* reflection_model){
    Vec3f point, N;
    Material material;

    if (!scene_intersect(orig, dir, objects, point, N, material)) {
        return Vec3f(0.2, 0.7, 0.8); // background color
    }

    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for (size_t i=0; i<lights.size(); i++) {
        Vec3f light_dir      = (lights[i].position - point).normalize();
        diffuse_light_intensity  += lights[i].intensity * std::max(0.f, light_dir*N);
        if (reflection_model == "Phong") {
            specular_light_intensity += powf(std::max(0.f, -reflect(-light_dir, N)*dir), material.get_specular_exponent())*lights[i].intensity;
        }
        if (reflection_model == "Blinn-Phong") {
            Vec3f H = (light_dir - dir).normalize();
            specular_light_intensity += powf(std::max(0.f, H*N), material.get_specular_exponent())*lights[i].intensity;
        }           
    }

    if (reflection_model == "None") {
        return material.get_diffuse_color() * diffuse_light_intensity * material.get_albedo()[0];
    }
    else{
        return material.get_diffuse_color() * diffuse_light_intensity * material.get_albedo()[0]
             + Vec3f(1., 1., 1.) * specular_light_intensity * material.get_albedo()[1];
    }
}

void render(const std::vector<Object*> &objects, const std::vector<Light> &lights,char* reflection_model = "None") {
    const int width    = 1024;
    const int height   = 768;
    const int fov      = M_PI/2.;
    std::vector<Vec3f> framebuffer(width*height);
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            float x =  (2*(i + 0.5)/(float)width  - 1)*tan(fov/2.)*width/(float)height;
            float y = -(2*(j + 0.5)/(float)height - 1)*tan(fov/2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            framebuffer[i+j*width] = cast_ray(Vec3f(0,0,0), dir, objects, lights,reflection_model);
        }
    }

    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm");
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}



int main() {
    Material      ivory(Vec3f(0.4, 0.4, 0.3), Vec4f(0.9,  0.5, 0.1, 0.0), 50., 1.);
    Material red_rubber(Vec3f(0.3, 0.1, 0.1), Vec4f(1.4,  0.3, 0.0, 0.0), 10., 1.);

    std::vector<Object*> objects;

    objects.push_back(new Sphere(Vec3f(-3,    3,   -16), 2,      ivory));
    objects.push_back(new Sphere(Vec3f(-5.0, -5.5, -12), 5, red_rubber));
    objects.push_back(new Sphere(Vec3f( 1.5, -0.5, -18), 3, red_rubber));
    objects.push_back(new Sphere(Vec3f( 7,    5,   -18), 4,      ivory));


    std::vector<Light>  lights;
    lights.push_back(Light(Vec3f(-20, 20,  20), 1.5));


    // le modèle de réflection par défaut est "None"
    // Vous pouvez choisir parmis "Phong" et "Blinn-Phong
    render(objects, lights,"Blinn-Phong");


    return 0;
}