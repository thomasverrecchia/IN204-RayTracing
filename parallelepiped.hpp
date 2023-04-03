#ifndef __PARALLELEPIPED_HPP__
#define __PARALLELEPIPED_HPP__
#include "object.hpp"
#include "vectors.hpp"

#define EPSILON 0.0001

class Parallelepiped : public Object {
public:
    Parallelepiped(Vec3f position, Vec3f size, Material material,
                   float angle_x = 0.f, float angle_y = 0.f, float angle_z = 0.f)
        : Object(material), m_position(position), m_size(size),
          m_half_size(size / 2.f) {

        // Calcul des vecteurs de direction pour les axes x, y et z
        m_direction_x = Vec3f(std::cos(angle_y) * std::cos(angle_z),
                              std::sin(angle_x) * std::sin(angle_y) * std::cos(angle_z) - std::cos(angle_x) * std::sin(angle_z),
                              std::cos(angle_x) * std::sin(angle_y) * std::cos(angle_z) + std::sin(angle_x) * std::sin(angle_z));
        m_direction_y = Vec3f(std::cos(angle_y) * std::sin(angle_z),
                              std::sin(angle_x) * std::sin(angle_y) * std::sin(angle_z) + std::cos(angle_x) * std::cos(angle_z),
                              std::cos(angle_x) * std::sin(angle_y) * std::sin(angle_z) - std::sin(angle_x) * std::cos(angle_z));
        m_direction_z = Vec3f(-std::sin(angle_y),
                              std::sin(angle_x) * std::cos(angle_y),
                              std::cos(angle_x) * std::cos(angle_y));
    }

    bool ray_intersect(const Vec3f& orig, const Vec3f& dir, float& t0) const override {
        // Transformation du rayon dans le repère local du parallélépipède
        Vec3f local_orig = global_to_local(orig - m_position);
        Vec3f local_dir = global_to_local(dir);

        // Calcul des intersections entre le rayon et les plans du parallélépipède
        float tmin = (-local_orig.x - m_half_size.x) / local_dir.x;
        float tmax = (-local_orig.x + m_half_size.x) / local_dir.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (-local_orig.y - m_half_size.y) / local_dir.y;
        float tymax = (-local_orig.y + m_half_size.y) / local_dir.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax)) return false;

        if (tymin > tmin) tmin = tymin;

        if (tymax < tmax) tmax = tymax;

        float tzmin = (-local_orig.z - m_half_size.z) / local_dir.z;
        float tzmax = (-local_orig.z + m_half_size.z) / local_dir.z;

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
        Vec3f local_point = global_to_local(point - m_position);
        Vec3f normal;
        if (std::abs(local_point.x + m_half_size.x) < EPSILON) {
            normal = local_to_global(Vec3f(-1, 0, 0));
        } else if (std::abs(local_point.x - m_half_size.x) < EPSILON) {
            normal = local_to_global(Vec3f(1, 0, 0));
        } else if (std::abs(local_point.y + m_half_size.y) < EPSILON) {
            normal = local_to_global(Vec3f(0, -1, 0));
        } else if (std::abs(local_point.y - m_half_size.y) < EPSILON) {
            normal = local_to_global(Vec3f(0, 1, 0));
        } else if (std::abs(local_point.z + m_half_size.z) < EPSILON) {
            normal = local_to_global(Vec3f(0, 0, -1));
        } else {
            normal = local_to_global(Vec3f(0, 0, 1));
        }

        return normal.normalize();
    }


private:
    Vec3f m_position;
    Vec3f m_size;
    Vec3f m_direction_x;
    Vec3f m_direction_y;
    Vec3f m_direction_z;
    Vec3f m_half_size;

    Vec3f global_to_local(const Vec3f& v) const {
        return Vec3f(v * m_direction_x, v * m_direction_y, v * m_direction_z);
    }

    Vec3f local_to_global(const Vec3f& v) const {
        return Vec3f(v.x * m_direction_x.x + v.y * m_direction_y.x + v.z * m_direction_z.x,
                     v.x * m_direction_x.y + v.y * m_direction_y.y + v.z * m_direction_z.y,
                     v.x * m_direction_x.z + v.y * m_direction_y.z + v.z * m_direction_z.z);
    }
};




#endif
