#ifndef __PLANE_HPP__
#define __PLANE_HPP__
#include "object.hpp"
#include "vectors.hpp"

class Plane : public Object {
public:
    Plane(Vec3f normal, float distance, Material material)
        : Object(material), m_normal(normal), m_distance(distance) {}

    // Méthode pour calculer l'intersection d'un rayon avec le plan
    bool ray_intersect(const Vec3f& orig, const Vec3f& dir, float& t0) const override {
        float denom = m_normal * dir;
        if (std::abs(denom) > 1e-6) {
            Vec3f orig_to_plane = m_normal * m_distance - orig;
            t0 = orig_to_plane * m_normal / denom;
            return t0 >= 0;
        }
        return false;
    }

    Vec3f get_position() const override { return m_normal * m_distance; }
    void set_position(const Vec3f& position) override { m_distance = position * m_normal; }

    Vec3f get_normal(const Vec3f& intersection_point) const override {
        if( (intersection_point + m_normal).norm() <= (intersection_point + Vec3f(0,0,0)).norm() ) return m_normal ;
        else return -m_normal;
    }


private:
    Vec3f m_normal;
    float m_distance;
};



class CheckerboardPlane : public Plane {
public:
    CheckerboardPlane(Vec3f normal, float distance,
                      Material material1, Material material2,
                      float size)
        : Plane(normal, distance, material1), m_material1(material1), m_material2(material2), m_size(size) {}

    Material get_material(const Vec3f& intersection_point) const override {
        int x = static_cast<int>(std::floor(intersection_point.x / m_size));
        int y = static_cast<int>(std::floor(intersection_point.y / m_size));
        int z = static_cast<int>(std::floor(intersection_point.z / m_size));
        if ((x + y + z) % 2 == 0) {
            return m_material1;
        } else {
            return m_material2;
        }
    }

private:
    Material m_material1;
    Material m_material2;
    float m_size;
};




#endif