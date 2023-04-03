#ifndef __CUBE_HPP__
#define __CUBE_HPP__

#include "object.hpp"
#include "vectors.hpp"

class Cube : public Object {
public:
    Cube(Vec3f center, float sideLength, Material material) 
        : Object(material), center(center), sideLength(sideLength) {}

    // Méthode pour calculer l'intersection d'un rayon avec le cube
    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const override {
        float tmin = (center.x - sideLength / 2 - orig.x) / dir.x;
        float tmax = (center.x + sideLength / 2 - orig.x) / dir.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (center.y - sideLength / 2 - orig.y) / dir.y;
        float tymax = (center.y + sideLength / 2 - orig.y) / dir.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax)) return false;

        if (tymin > tmin) tmin = tymin;

        if (tymax < tmax) tmax = tymax;

        float tzmin = (center.z - sideLength / 2 - orig.z) / dir.z;
        float tzmax = (center.z + sideLength / 2 - orig.z) / dir.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax)) return false;

        if (tzmin > tmin) tmin = tzmin;

        if (tzmax < tmax) tmax = tzmax;

        t0 = tmin;
        return true;
    }

    Vec3f get_position() const override { return center; }
    void set_position(const Vec3f& position) override { center = position; }

    Vec3f get_normal(const Vec3f& point) const override {
        if (point.x > center.x - sideLength / 2 && point.x < center.x + sideLength / 2) {
            if (point.y > center.y - sideLength / 2 && point.y < center.y + sideLength / 2) {
                return Vec3f(0, 0, point.z > center.z ? 1 : -1);
            } else if (point.z > center.z - sideLength / 2 && point.z < center.z + sideLength / 2) {
                return Vec3f(0, point.y > center.y ? 1 : -1, 0);
            }
        } else if (point.y > center.y - sideLength / 2 && point.y < center.y + sideLength / 2 && point.z > center.z - sideLength / 2 && point.z < center.z + sideLength / 2) {
            return Vec3f(point.x > center.x ? 1 : -1, 0, 0);
        }
        return Vec3f();
    }

private:
    Vec3f center;
    float sideLength;

};

#endif
