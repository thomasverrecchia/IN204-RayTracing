#include <limits>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "vectors.hpp"
#include "sphere.hpp"
#include "object.hpp"
#include "light.hpp"
#include "parallelepiped.hpp"
#include "plane.hpp"
#include "omp.h"




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

Vec3f refract(const Vec3f &I, const Vec3f &N, const float &refractive_index) { // Snell's law
    float cosi = - std::max(-1.f, std::min(1.f, I*N));
    float etai = 1, etat = refractive_index;
    Vec3f n = N;
    if (cosi < 0) { // si le rayon est à l'intérieur de l'objet, on échange les indices et on inverse la normale pour obtenir le résultat correct
        cosi = -cosi;
        std::swap(etai, etat); n = -N;
    }
    float eta = etai / etat;
    float k = 1 - eta*eta*(1 - cosi*cosi);
    return k < 0 ? Vec3f(0,0,0) : I*eta + n*(eta * cosi - sqrtf(k));
}


Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const std::vector<Object*> &objects, const std::vector<Light> &lights,
char* reflection_model, size_t depth=0){
    Vec3f point, N;
    Material material;

    if (depth>4 || !scene_intersect(orig, dir, objects, point, N, material)) {
        return Vec3f(0.3, 0.3, 0.3); // fond gris
    }

    Vec3f reflect_dir = reflect(dir, N).normalize();
    Vec3f reflect_orig = reflect_dir*N < 0 ? point - N*1e-3 : point + N*1e-3; // offset the original point to avoid occlusion by the object itself
    Vec3f reflect_color = cast_ray(reflect_orig, reflect_dir, objects, lights, reflection_model, depth + 1);

    Vec3f refract_dir = refract(dir, N, material.get_refractive_index()).normalize();
    Vec3f refract_orig = refract_dir*N < 0 ? point - N*1e-3 : point + N*1e-3;
    Vec3f refract_color = cast_ray(refract_orig, refract_dir, objects, lights, reflection_model, depth + 1);


    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for (size_t i=0; i<lights.size(); i++) {
        Vec3f light_dir      = (lights[i].position - point).normalize();

        float light_distance = (lights[i].position - point).norm();

        Vec3f shadow_orig = light_dir*N < 0 ? point + N*1e-3 : point + N*1e-3; // checking if the point lies in the shadow of the lights[i]

        Vec3f shadow_pt, shadow_N;
        Material tmpmaterial;
        if (scene_intersect(shadow_orig, light_dir, objects, shadow_pt, shadow_N, tmpmaterial) && (shadow_pt-shadow_orig).norm() < light_distance)
            continue;
        
        
        
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
            + Vec3f(1., 1., 1.)*specular_light_intensity * material.get_albedo()[1] + reflect_color*material.get_albedo()[2] + refract_color*material.get_albedo()[3];
    }
}

void render(const std::vector<Object*> &objects, const std::vector<Light> &lights,char* reflection_model = "None") {
    const int width    = 1024;
    const int height   = 768;
    const int fov      = M_PI/2.;
    std::vector<Vec3f> framebuffer(width*height);

    #pragma omp parallel for
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
            objects.push_back(new Parallelepiped(center, size, material));


        }else if (type == "Lights") {
            float intensity = stof(tokens[4]);
            lights.push_back(Light(center, intensity));
        }
    }
}


int main() {
    Material      ivory(Vec3f(0.4, 0.4, 0.3), Vec4f(0.9,  0.5, 0.1, 0.0), 50., 1.);
    Material red_rubber(Vec3f(0.3, 0.1, 0.1), Vec4f(1.4,  0.3, 0.0, 0.0), 10., 1.);
    Material     mirror(Vec3f(1.0, 1.0, 1.0), Vec4f(0.0, 16.0, 0.8, 0.0), 1425., 1.);
    Material      glass(Vec3f(0.6, 0.7, 0.8), Vec4f(0.0,  0.9, 0.1, 0.8), 125., 1.5);

    std::vector<Object*> objects;
    std::vector<Light>  lights;

    //DEFINITION DU PLAN EN HARD

    objects.push_back(  new Plane(  Vec3f(0, 1, 0), //Normale du plan
                                    -4,             //Distance entre l'origine (observateur) et la normale (au sens de plus petite distance entre un point du plan et l'origine)
                                    ivory));        //Matériau du plan



    //Autres objets test en hard
    objects.push_back(new Sphere(Vec3f(0,1,-15), 2, mirror));
    objects.push_back(new Sphere(Vec3f(3,1,-10), 1, glass));
    
    
    #ifdef __linux__

        FILE *fp;
        char file_path[1024];

        // Utiliser zenity pour demander à l'utilisateur de sélectionner un fichier
        char command[1024] = "zenity --file-selection --title=\"Sélectionner un fichier CSV\"";

        fp = popen(command, "r");

        if (fp == NULL) {
            std::cerr << "Erreur : impossible d'exécuter la commande" << std::endl;
            return 1;
        }

        fgets(file_path, 1024, fp);

        pclose(fp);



        std::cout << "Chemin d'accès au fichier : " << file_path << std::endl;

        load_csv("config1.csv", objects, lights);
        render(objects, lights,"Phong");
    
    #elif _WIN32

        // Utiliser la fonction de Windows pour demander à l'utilisateur de sélectionner un fichier
        OPENFILENAME ofn;
        char filename[1024] = "";

        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = "CSV Files (*.csv)\0*.csv\0";
        ofn.lpstrFile = filename;
        ofn.nMaxFile = sizeof(filename);
        ofn.lpstrTitle = "Sélectionner un fichier CSV";
        ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn)) {
            std::cout << "Chemin d'accès au fichier : " << filename << std::endl;
            load_csv(filename, objects, lights);
            render(objects, lights,"Phong");
        }

    #elif __APPLE__
        std::cout << "OS non supporté, veuillez saisir le fichier manuellement" << std::endl;
      
    #endif
    
    // Si le code prècédent ne marche pas sur votre système d'exploitation mettre en commentaire les #ifdef et 
    // décommenter le code ci-dessous

    
    /*

    // On rajoute des Objets
    objects.push_back(new Sphere(Vec3f(-3, 0, -16), 2, ivory));
    objects.push_back(new Sphere(Vec3f(-1.0, -1.5, -12), 2, red_rubber));
    objects.push_back(new Sphere(Vec3f(1.5, -0.5, -18), 3, red_rubber));
    objects.push_back(new Sphere(Vec3f(7, 5, -18), 4, ivory));

    // On rajoute des lumières
    lights.push_back(Light(Vec3f(-20, 20, 20), 1.5));

    // On rajoute un plan
    objects.push_back(new Plane(Vec3f(0, 1, 0), -4, ivory));

    // On lance le rendu
    render(objects, lights,"Phong");

    */



    return 0;
}
