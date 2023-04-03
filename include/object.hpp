#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__
#include "vectors.hpp"

class Material {
public:
    Material() {}
    Material(const Vec3f& diffuse_color, const Vec4f& albedo, float specular_exponent, float refractive_index)
        : m_diffuseColor(diffuse_color), m_albedo(albedo), m_specularExponent(specular_exponent), m_refractiveIndex(refractive_index) {}


    // Getters et setters pour les propriétés du matériau
    Vec3f get_diffuse_color() const { return m_diffuseColor; }
    void set_diffuse_color(const Vec3f& diffuse_color) { m_diffuseColor = diffuse_color; }
    Vec4f get_albedo() const { return m_albedo; }
    void set_albedo(const Vec4f& albedo) { m_albedo = albedo; }
    float get_specular_exponent() const { return m_specularExponent; }
    void set_specular_exponent(float specular_exponent) { m_specularExponent = specular_exponent; }
    float get_refractive_index() const { return m_refractiveIndex; }
    void set_refractive_index(float refractive_index) { m_refractiveIndex = refractive_index; }

private:
    Vec3f m_diffuseColor;
    Vec4f m_albedo;
    float m_specularExponent;
    float m_refractiveIndex;
};



class Object {
public:
    Object(Material material) : m_material(material) {}
    virtual ~Object() {}

    // Méthode pour calculer l'intersection d'un rayon avec l'objet
    virtual bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const = 0;

    // Getters et setters pour les propriétés communes à tous les objets
    virtual Vec3f get_position() const = 0;
    virtual void set_position(const Vec3f& position) = 0;

    virtual Material get_material(const Vec3f& intersection_point) const { return m_material; }
    virtual void set_material(const Material& material) { m_material = material; }

    // Méthode pour obtenir le vecteur normal au point d'intersection entre un rayon et l'objet
    virtual Vec3f get_normal(const Vec3f& intersection_point) const = 0;

private:
    Material m_material;
};





#endif