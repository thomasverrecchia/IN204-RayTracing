#ifndef __VECTORS_HPP__
#define __VECTORS_HPP__
#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

// Définition de la structure vec avec une taille de dimension et un type de données génériques
template <size_t DIM, typename T> struct vec {
    // Constructeur qui initialise les valeurs du vecteur à zéro
    vec() { for (size_t i=DIM; i--; data_[i] = T()); }
    // Opérateur d'indexation pour accéder aux éléments individuels du vecteur et les modifier
    T& operator[](const size_t i)       { assert(i<DIM); return data_[i]; }
    const T& operator[](const size_t i) const { assert(i<DIM); return data_[i]; }
private:
    // Matrice de données privées qui stocke les valeurs du vecteur
    T data_[DIM];
};

// Alias pour les vecteurs couramment utilisés Vec2f, Vec3f, Vec3i et Vec4f
typedef vec<2, float> Vec2f;
typedef vec<3, float> Vec3f;
typedef vec<3, int  > Vec3i;
typedef vec<4, float> Vec4f;

// Spécialisation de la structure vec pour les vecteurs 2D
template <typename T> struct vec<2,T> {
    // Constructeur par défaut qui initialise les valeurs du vecteur à zéro
    vec() : x(T()), y(T()) {}
    // Constructeur qui permet d'initialiser les valeurs du vecteur avec des paramètres donnés
    vec(T X, T Y) : x(X), y(Y) {}
    // Constructeur qui permet de convertir un vecteur de type U en un vecteur de type T
    template <class U> vec<2,T>(const vec<2,U> &v);
    // Opérateur d'indexation pour accéder aux éléments individuels du vecteur et les modifier
    T& operator[](const size_t i)       { assert(i<2); return i<=0 ? x : y; }
    const T& operator[](const size_t i) const { assert(i<2); return i<=0 ? x : y; }
    // Les valeurs du vecteur
    T x,y;
};

// Spécialisation de la structure vec pour les vecteurs 3D
template <typename T> struct vec<3,T> {
    // Constructeur par défaut qui initialise les valeurs du vecteur à zéro
    vec() : x(T()), y(T()), z(T()) {}
    // Constructeur qui permet d'initialiser les valeurs du vecteur avec des paramètres donnés
    vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
    // Opérateur d'indexation pour accéder aux éléments individuels du vecteur et les modifier
    T& operator[](const size_t i)       { assert(i<3); return i<=0 ? x : (1==i ? y : z); }
    const T& operator[](const size_t i) const { assert(i<3); return i<=0 ? x : (1==i ? y : z); }
    // Calcule la norme du vecteur
    float norm() { return std::sqrt(x*x+y*y+z*z); }
    // Normalise le vecteur
    vec<3,T> & normalize(T l=1) { *this = (*this)*(l/norm()); return *this; }
    // Les valeurs du vecteur
    T x,y,z;
};

// Spécialisation de la structure vec pour les vecteurs 4D
template <typename T> struct vec<4,T> {
    // Constructeur par défaut qui initialise les valeurs du vecteur à zéro
    vec() : x(T()), y(T()), z(T()), w(T()) {}
    // Constructeur qui permet d'initialiser les valeurs du vecteur avec des paramètres donnés
    vec(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}
    // Opérateur d'indexation pour accéder aux éléments individuels du vecteur et les modifier
    T& operator[](const size_t i)       { assert(i<4); return i<=0 ? x : (1==i ? y : (2==i ? z : w)); }
    const T& operator[](const size_t i) const { assert(i<4); return i<=0 ? x : (1==i ? y : (2==i ? z : w)); }
    // Les valeurs du vecteur
    T x,y,z,w;
};


// Surcharge de l'opérateur de multiplication pour les vecteurs de même dimension
template<size_t DIM,typename T> T operator*(const vec<DIM,T>& lhs, const vec<DIM,T>& rhs) {
    // Initialise la variable ret à zéro
    T ret = T();
    // Effectue le produit scalaire des deux vecteurs et stocke le résultat dans ret
    for (size_t i=DIM; i--; ret+=lhs[i]*rhs[i]);
    // Retourne le résultat
    return ret;
}

// Surcharge de l'opérateur d'addition pour les vecteurs de même dimension
template<size_t DIM,typename T>vec<DIM,T> operator+(vec<DIM,T> lhs, const vec<DIM,T>& rhs) {
    // Ajoute chaque élément du vecteur rhs à celui du vecteur lhs correspondant
    for (size_t i=DIM; i--; lhs[i]+=rhs[i]);
    // Retourne le vecteur résultant
    return lhs;
}

// Surcharge de l'opérateur de soustraction pour les vecteurs de même dimension
template<size_t DIM,typename T>vec<DIM,T> operator-(vec<DIM,T> lhs, const vec<DIM,T>& rhs) {
    // Soustrait chaque élément du vecteur rhs à celui du vecteur lhs correspondant
    for (size_t i=DIM; i--; lhs[i]-=rhs[i]);
    // Retourne le vecteur résultant
    return lhs;
}

// Surcharge de l'opérateur de multiplication pour un vecteur et un scalaire
template<size_t DIM,typename T,typename U> vec<DIM,T> operator*(const vec<DIM,T> &lhs, const U& rhs) {
    // Initialise un nouveau vecteur ret
    vec<DIM,T> ret;
    // Multiplie chaque élément du vecteur lhs par le scalaire rhs et stocke le résultat dans le vecteur ret correspondant
    for (size_t i=DIM; i--; ret[i]=lhs[i]*rhs);
    // Retourne le vecteur résultant
    return ret;
}

// Surcharge de l'opérateur de négation pour un vecteur
template<size_t DIM,typename T> vec<DIM,T> operator-(const vec<DIM,T> &lhs) {
    // Retourne le vecteur lhs multiplié par -1
    return lhs*T(-1);
}

// Calcule le produit vectoriel de deux vecteurs 3D
template <typename T> vec<3,T> cross(vec<3,T> v1, vec<3,T> v2) {
    // Calcule les composantes x, y et z du vecteur résultant en utilisant la formule du produit vectoriel
    return vec<3,T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

// Surcharge de l'opérateur de flux de sortie pour afficher un vecteur
template <size_t DIM, typename T> std::ostream& operator<<(std::ostream& out, const vec<DIM,T>& v) {
    // Parcourt chaque élément du vecteur et les affiche séparés par un espace
    for(unsigned int i=0; i<DIM; i++) {
        out << v[i] << " " ;
    }
    // Retourne le flux de sortie
    return out ;
}


#endif