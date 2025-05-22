#pragma once
#ifndef CVECTOR_H    
#define CVECTOR_H

#include "CEuler.h"  // �滻ǰ������Ϊ����ͷ�ļ���ȷ��CEuler����ɼ���
#include <cmath>     // ��ӱ�׼��ͷ�ļ�
#include <stdexcept> // �쳣����

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif


class CVector2 {
public:
    float u, v;

    CVector2() : u(0.0f), v(0.0f) {}
    CVector2(float u, float v) : u(u), v(v) {}
    
    operator float* () { return &u; }
    operator const float* () const { return &u; }
};

class CVector {
public:
    // ��Ա����
    float x, y, z;
    CVector() : x(0.0f), y(0.0f), z(0.0f) {} // ֱ�������ڶ���Ĭ�Ϲ��캯��
    CVector(float x, float y, float z) : x(x), y(y), z(z) {}
    // ��������
    void Set(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    // ���������
    CVector operator+(const CVector& p) const {
        return CVector(x + p.x, y + p.y, z + p.z);
    }
    CVector operator-(const CVector& p) const {
        return CVector(x - p.x, y - p.y, z - p.z);
    }
    CVector& operator=(const CVector& p) {
        if (this != &p) {
            x = p.x;
            y = p.y;
            z = p.z;
        }
        return *this;
    }
    bool operator==(const CVector& p) const {
        return (x == p.x) && (y == p.y) && (z == p.z);
    }
    bool operator!=(const CVector& p) const {
        return !(*this == p);
    }
    CVector operator*(float scalar) const {
        return CVector(x * scalar, y * scalar, z * scalar);
    }

    // ��Ԫ������������ǰ�˷���
    friend CVector operator*(float scalar, const CVector& vec) {
        return CVector(vec.x * scalar, vec.y * scalar, vec.z * scalar);
    }

    // ��������
    float dotMul(const CVector& n) const {
        return x * n.x + y * n.y + z * n.z;
    }
    CVector crossMul(const CVector& n) const {
        return CVector(
            y * n.z - z * n.y,
            z * n.x - x * n.z,
            x * n.y - y * n.x
        );
    }

    // ������׼��
    void Normalize() {
        float length = len();
        if (length == 0) {
            throw std::runtime_error("Cannot normalize zero vector");
        }
        x /= length;
        y /= length;
        z /= length;
    }

    CVector Normalized() const {
        CVector temp(*this);
        temp.Normalize();
        return temp;
    }

    // ģ������
    float len() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // ͶӰ����
    CVector project(const CVector& n) const {
        float denominator = n.len() * n.len();
        if (denominator == 0) {
            throw std::invalid_argument("Projection onto zero vector");
        }
        float scalar = this->dotMul(n) / denominator;
        return n * scalar;
    }

    // �����нǣ����ȣ�
    float angleWith(const CVector& other) const {
        float lengthsProduct = len() * other.len();
        if (lengthsProduct == 0) {
            throw std::invalid_argument("Angle with zero vector");
        }
        float cosTheta = dotMul(other) / lengthsProduct;
        cosTheta = std::max(-1.0f, std::min(1.0f, cosTheta));
        return std::acos(cosTheta);
    }

    // �������
    float distanceTo(const CVector& other) const {
        return (*this - other).len();
    }

    // ת��Ϊŷ����
    CEuler ToEuler() const {
        CEuler euler;
        CVector dir = Normalized();
        // �����ǣ���X�ᣩ
        euler.p = std::asin(dir.y) * 180.0f / M_PI;
        // ����ǣ���Y�ᣩ
        if (std::fabs(dir.x) < 1e-6f && std::fabs(dir.z) < 1e-6f) {
            euler.h = 0.0f;
        }
        else {
            euler.h = -std::atan2f(dir.x, -dir.z) * 180.0f / M_PI;
        }
        // �������Ϊ0
        euler.b = 0.0f;
        euler.Normal(); // �淶���Ƕ�
        return euler;
    }

    // ����ת������ʽת��Ϊfloat����ָ�룩
    operator float* () {
        return &x;
    }

    void output() {
        printf("%f %f %f\n", this->x, this->y, this->z);
    }

    CVector operator-() const {
        return CVector(-x, -y, -z);
    }

    
};

#endif // CVECTOR_H