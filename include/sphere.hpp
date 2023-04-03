#ifndef __SPHERE_HPP__
#define __SPHERE_HPP__
#include "object.hpp"
#include "vectors.hpp"

class Sphere : public Object {
public:
    Sphere(Vec3f center, float radius, Material material) 
        : Object(material), center(center), radius(radius) {}

    // Méthode pour calculer l'intersection d'un rayon avec la sphère
    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const override {
        Vec3f L = center - orig;
        float tca = L*dir;
        float d2 = L*L - tca*tca;
        if (d2 > radius*radius) return false;
        float thc = sqrtf(radius*radius - d2);
        t0       = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        return true;
    }

    Vec3f get_position() const override { return center; }
    void set_position(const Vec3f& position) override { center = position; }

    Vec3f get_normal(const Vec3f& point) const override {
        return (point - center).normalize();
    }

private:
    Vec3f center;
    float radius;

};


#endif