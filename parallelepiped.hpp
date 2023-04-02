#ifndef __PARALLELEPIPED_HPP__
#define __PARALLELEPIPED_HPP__
#include "object.hpp"
#include "vectors.hpp"

#define EPSILON 0.0001

class Parallelepiped : public Object {
public:
    Parallelepiped(Vec3f position, Vec3f size, Material material)
        : Object(material), m_position(position), m_size(size) {}

    bool ray_intersect(const Vec3f& orig, const Vec3f& dir, float& t0) const override {
        // Calcul des intersections entre le rayon et les plans du parallélépipède
        float tmin = (m_position.x - orig.x) / dir.x;
        float tmax = (m_position.x + m_size.x - orig.x) / dir.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (m_position.y - orig.y) / dir.y;
        float tymax = (m_position.y + m_size.y - orig.y) / dir.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax)) return false;

        if (tymin > tmin) tmin = tymin;

        if (tymax < tmax) tmax = tymax;

        float tzmin = (m_position.z - orig.z) / dir.z;
        float tzmax = (m_position.z + m_size.z - orig.z) / dir.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax)) return false;

        if (tzmin > tmin) tmin = tzmin;

        if (tzmax < tmax) tmax = tzmax;

        // Vérification de la validité des intersections
        if (tmin < 0) {
            t0 = tmax;
        } else {
            t0 = tmin;
        }

        return true;
    }

    Vec3f get_position() const override { return m_position; }
    void set_position(const Vec3f& position) override { m_position = position; }

    Vec3f get_normal(const Vec3f& point) const override {
        // Calcul de la normale à partir du point d'intersection
        if (point.x < m_position.x + EPSILON) {
            return Vec3f(-1, 0, 0);
        } else if (point.x > m_position.x + m_size.x - EPSILON) {
            return Vec3f(1, 0, 0);
        } else if (point.y < m_position.y + EPSILON) {
            return Vec3f(0, -1, 0);
        } else if (point.y > m_position.y + m_size.y - EPSILON) {
            return Vec3f(0, 1, 0);
        } else if (point.z < m_position.z + EPSILON) {
            return Vec3f(0, 0, -1);
        } else {
            return Vec3f(0, 0, 1);
        }
    }

private:
    Vec3f m_position;
    Vec3f m_size;
};

#endif
