#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__
#include <cmath>

class Mat4f {
public:
    // Constructeurs
    Mat4f();
    Mat4f(float m00, float m01, float m02, float m03,
          float m10, float m11, float m12, float m13,
          float m20, float m21, float m22, float m23,
          float m30, float m31, float m32, float m33);
    
    // Fonctions membres
    Mat4f operator*(const Mat4f& other) const;
    Vec3f operator*(const Vec3f& vec) const;
    void translate(float x, float y, float z);
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);
    void scale(float x, float y, float z);
    
    // Variables membres
    float m[16];
};


// Constructeur par défaut
Mat4f::Mat4f() {
    for (int i = 0; i < 16; i++) {
        m[i] = 0.0f;
    }
}

// Constructeur avec initialisation des éléments de la matrice
Mat4f::Mat4f(float m00, float m01, float m02, float m03,
              float m10, float m11, float m12, float m13,
              float m20, float m21, float m22, float m23,
              float m30, float m31, float m32, float m33) {
    m[0] = m00; m[1] = m01; m[2] = m02; m[3] = m03;
    m[4] = m10; m[5] = m11; m[6] = m12; m[7] = m13;
    m[8] = m20; m[9] = m21; m[10] = m22; m[11] = m23;
    m[12] = m30; m[13] = m31; m[14] = m32; m[15] = m33;
}

// Opérateur de multiplication de matrices
Mat4f Mat4f::operator*(const Mat4f& other) const {
    Mat4f result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += m[i * 4 + k] * other.m[k * 4 + j];
            }
            result.m[i * 4 + j] = sum;
        }
    }
    return result;
}

// Opérateur de multiplication d'une matrice par un vecteur
Vec3f Mat4f::operator*(const Vec3f& vec) const {
    float x = m[0] * vec.x + m[1] * vec.y + m[2] * vec.z + m[3];
    float y = m[4] * vec.x + m[5] * vec.y + m[6] * vec.z + m[7];
    float z = m[8] * vec.x + m[9] * vec.y + m[10] * vec.z + m[11];
    return Vec3f(x, y, z);
}

// Fonction pour effectuer une translation sur la matrice
void Mat4f::translate(float x, float y, float z) {
    Mat4f translation(1.0f, 0.0f, 0.0f, x,
                      0.0f, 1.0f, 0.0f, y,
                      0.0f, 0.0f, 1.0f, z,
                      0.0f, 0.0f, 0.0f, 1.0f);
    *this = translation * (*this);
}

// Fonction pour effectuer une rotation autour de l'axe X sur la matrice
void Mat4f::rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    Mat4f rotation(1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, c, -s, 0.0f,
                   0.0f, s, c, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
    *this = rotation * (*this);
}

// Fonction pour effectuer une rotation autour de l'axe Y sur la matrice
void Mat4f::rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    Mat4f rotation(c, 0.0f, s, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   -s, 0.0f, c, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
    *this = rotation * (*this);
}

// Fonction pour effectuer une rotation autour de l'axe Z sur la matrice
void Mat4f::rotateZ(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    Mat4f rotation(c, -s, 0.0f, 0.0f,
                   s, c, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
    *this = rotation * (*this);
}

// Fonction pour effectuer une mise à l'échelle sur la matrice
void Mat4f::scale(float x, float y, float z) {
    Mat4f scaling(x, 0.0f, 0.0f, 0.0f,
                  0.0f, y, 0.0f, 0.0f,
                  0.0f, 0.0f, z, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f);
    *this = scaling * (*this);
}



#endif