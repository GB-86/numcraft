#include "vector3.h"
#include "math.h"
// Default constructor
using namespace std;
Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

// Parameterized constructor
Vector3::Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

// Addition
Vector3 Vector3::add(const Vector3& v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
}

// Subtraction
Vector3 Vector3::sub(const Vector3& v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
}

// Multiplication by a scalar
Vector3 Vector3::mul(double scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}
double Vector3::dot(const Vector3& v) const {
    return x * v.x+  y * v.y    +z * v.z;
}
// Normalization
Vector3 Vector3::normalize() const {
    double invLen = q_rsqrt(x*x+y*y+z*z);
    return Vector3(x * invLen, y * invLen, z * invLen);
}
unsigned int Vector3::toColor() const {


  //  00000000 00000000 00000000 11111000
  //  00000000 00000000 11111100 00000000
  //  00000000 11111000 00000000 00000000

    return ((((unsigned int)x)&255)<<16)   |((((unsigned int)y)&255)<<8)|     ((unsigned int)z)&255;
}
// Length of the vector
double Vector3::length() const {
    
    return (x * x + y * y + z * z);
}
double Vector3::squareLength() const {
    return x * x + y * y + z * z;
}
Vector3 gradient(Vector3 v1 ,Vector3 v2,double t){
    return v1.mul(1-t).add(v2.mul(t));
}
