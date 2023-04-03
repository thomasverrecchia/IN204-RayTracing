#include <limits>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include "vectors.hpp"
#include "sphere.hpp"
#include "object.hpp"
#include "light.hpp"
#include "parallelepiped.hpp"
#include "plane.hpp"



bool scene_intersect(const Vec3f &orig, const Vec3f &dir, const std::vector<Object*> &objects, Vec3f &hit, Vec3f &N, Material &material) {
    float closest_dist = std::numeric_limits<float>::max();
    for (const auto& object : objects) {
        float dist_i;
        if (object->ray_intersect(orig, dir, dist_i) && dist_i < closest_dist) {
            closest_dist = dist_i;
            hit = orig + dir*dist_i;
            N = object->get_normal(hit);
            material = object->get_material(hit);
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
            specular_light_intensity += powf(std::max(0.f, reflect(light_dir, N)*dir), material.get_specular_exponent())*lights[i].intensity;
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

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void load_csv(const std::string& filename, std::vector<Object*>& objects, std::vector<Light>& lights) {
    std::ifstream file(filename);
    std::string line;

    getline(file, line); // skip header

    while (getline(file, line)) {
        std::vector<std::string> tokens = split(line, ';');

        std::string type = tokens[0];

        std::string center_str = tokens[1];
        center_str.erase(remove(center_str.begin(), center_str.end(), '('), center_str.end());
        center_str.erase(remove(center_str.begin(), center_str.end(), ')'), center_str.end());
        std::vector<std::string> center_tokens = split(center_str, ',');
        Vec3f center(stof(center_tokens[0]), stof(center_tokens[1]), stof(center_tokens[2]));


        Material material(Vec3f(0,0,0), Vec4f(0,0,0,0), 0, 0);
        if (type == "Sphere") {
            float radius = stof(tokens[2]);
            std::string material_str = trim(tokens[3]);
            if (material_str == "ivory") {
                material = Material(Vec3f(0.4, 0.4, 0.3), Vec4f(0.6,  0.3, 0.1, 0.0), 50., 1.);
            } else if (material_str == "red_rubber") {
                material = Material(Vec3f(0.3, 0.1, 0.1), Vec4f(0.9,  0.1, 0.0, 0.0), 10., 1.);
            }
            objects.push_back(new Sphere(center, radius, material));
        }else if (type == "Parallelepiped"){
            std::string material_str = trim(tokens[3]);
            if (material_str == "ivory") {
                material = Material(Vec3f(0.4, 0.4, 0.3), Vec4f(0.6,  0.3, 0.1, 0.0), 50., 1.);
            } else if (material_str == "red_rubber") {
                material = Material(Vec3f(0.3, 0.1, 0.1), Vec4f(0.9,  0.1, 0.0, 0.0), 10., 1.);
            }
            std::string size_str = tokens[5];
            size_str.erase(remove(size_str.begin(), size_str.end(), '('), size_str.end());
            size_str.erase(remove(size_str.begin(), size_str.end(), ')'), size_str.end());
            std::vector<std::string> size_tokens = split(size_str, ',');
            Vec3f size(stof(size_tokens[0]), stof(size_tokens[1]), stof(size_tokens[2]));
            float angle_x = stof(tokens[6]);
            float angle_y = stof(tokens[7]);
            float angle_z = stof(tokens[8]);
            objects.push_back(new Parallelepiped(center, size, material, angle_x, angle_y, angle_z));


        }else if (type == "Lights") {
            float intensity = stof(tokens[4]);
            lights.push_back(Light(center, intensity));
        }
    }
}


int main() {
    Material      ivory(Vec3f(0.4, 0.4, 0.3), Vec4f(0.9,  0.5, 0.1, 0.0), 50., 1.);
    Material red_rubber(Vec3f(0.3, 0.1, 0.1), Vec4f(1.4,  0.3, 0.0, 0.0), 10., 1.);

    std::vector<Object*> objects;

    //objects.push_back(new Sphere(Vec3f(-3,    3,   -16), 2,      ivory));
    //objects.push_back(new Sphere(Vec3f(-5.0, -5.5, -12), 5, red_rubber));
    //objects.push_back(new Sphere(Vec3f( 1.5, -0.5, -18), 3, red_rubber));
    objects.push_back(new Parallelepiped(Vec3f( 0,    0,   -10), Vec3f( 2,    2,   2), ivory, M_PI/7, M_PI/4, 0));
    objects.push_back(new CheckerboardPlane(Vec3f(0, 1, 0), -4, red_rubber, ivory, 2));
    //objects.push_back(new Plane(Vec3f(0, 1, 0), 10, red_rubber));

    

    std::vector<Light>  lights;
    //lights.push_back(Light(Vec3f(-20, 20, 20), 1.5));

    load_csv("config1.csv", objects, lights);
    
    // le modèle de réflection par défaut est "None"
    // Vous pouvez choisir parmis "Phong" et "Blinn-Phong"
    render(objects, lights,"Phong");


    return 0;
}
