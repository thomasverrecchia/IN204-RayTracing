#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__
#include "vectors.hpp"

class Object {
public:
    Object() {}
    virtual ~Object() {}

    // Méthode pour calculer l'intersection d'un rayon avec l'objet
    virtual bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const = 0;

    // Getters et setters pour les propriétés communes à tous les objets
    virtual Vec3f getPosition() const = 0;
    virtual void setPosition(const Vec3f& position) = 0;

    // Getters et setters pour les propriétés du matériau
    virtual float getRefractiveIndex() const = 0;
    virtual void setRefractiveIndex(float refractiveIndex) = 0;
    virtual Vec4f getAlbedo() = 0;
    virtual void setAlbedo(const Vec4f& albedo) = 0;
    virtual Vec3f getDiffuseColor() const = 0;
    virtual void setDiffuseColor(const Vec3f& diffuseColor) = 0;
    virtual float getSpecularExponent() const = 0;
    virtual void setSpecularExponent(float specularExponent) = 0;
};


#endif