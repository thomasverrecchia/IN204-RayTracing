#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__
#include "vectors.hpp"

class Light {
public:
    Vec3f position;
    float intensity;

    Light(const Vec3f& p, const float i) : position(p), intensity(i) {}
};


#endif