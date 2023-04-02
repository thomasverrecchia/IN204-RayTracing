#ifndef __SPHERE_HPP__
#define __SPHERE_HPP__
#include "object.hpp"
#include "vectors.hpp"

class Sphere : public Object {
public:
    Sphere(Vec3f center, float radius, Material material) 
        : m_center(center), m_radius(radius), m_material(material) {}

    // Méthode pour calculer l'intersection d'un rayon avec la sphère
    bool intersect(const Ray& ray, Intersection& intersection) const override {
        Vec3f oc = ray.origin - m_center;
        float a = dot(ray.direction, ray.direction);
        float b = 2.0 * dot(oc, ray.direction);
        float c = dot(oc, oc) - m_radius * m_radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return false;
        }

        float t1 = (-b - sqrt(discriminant)) / (2.0 * a);
        float t2 = (-b + sqrt(discriminant)) / (2.0 * a);

        if (t1 > t2) {
            std::swap(t1, t2);
        }

        if (t1 < 0) {
            t1 = t2;
            if (t1 < 0) {
                return false;
            }
        }

        intersection.distance = t1;
        intersection.point = ray.origin + t1 * ray.direction;
        intersection.normal = normalize(intersection.point - m_center);
        intersection.material = m_material;

        return true;
    }

    // Getters et setters pour les propriétés communes à tous les objets
    Vec3f getPosition() const override { return m_center; }
    void setPosition(const Vec3f& position) override { m_center = position; }
    Color getColor() const override { return Color(); } // La couleur est définie dans le matériau
    void setColor(const Color& color) override {}
    float getReflectivity() const override { return 0.0f; } // La réflectivité est définie dans le matériau
    void setReflectivity(float reflectivity) override {}

    // Getters et setters pour les propriétés du matériau
    float getRefractiveIndex() const override { return m_material.refractive_index; }
    void setRefractiveIndex(float refractiveIndex) override { m_material.refractive_index = refractiveIndex; }
    float* getAlbedo() override { return m_material.albedo; }
    void setAlbedo(float albedo[4]) override { memcpy(m_material.albedo, albedo, sizeof(float) * 4); }
    vec3 getDiffuseColor() const override { return m_material.diffuse_color; }
    void setDiffuseColor(const vec3& diffuseColor) override { m_material.diffuse_color = diffuseColor; }
    float getSpecularExponent() const override { return m_material.specular_exponent; }
    void setSpecularExponent(float specularExponent) override { m_material.specular_exponent = specularExponent; }

private:
    Vec3f m_center;
    float m_radius;
    Material m_material;
};


#endif