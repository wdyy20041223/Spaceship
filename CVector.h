#pragma once
#ifndef CVECTOR_H    
#define CVECTOR_H

#include "CEuler.h"  // 替换前向声明为包含头文件（确保CEuler定义可见）
#include <cmath>     // 添加标准库头文件
#include <stdexcept> // 异常处理

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
    // 成员变量
    float x, y, z;
    CVector() : x(0.0f), y(0.0f), z(0.0f) {} // 直接在类内定义默认构造函数
    CVector(float x, float y, float z) : x(x), y(y), z(z) {}
    // 基本操作
    void Set(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    // 运算符重载
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

    // 友元函数（标量在前乘法）
    friend CVector operator*(float scalar, const CVector& vec) {
        return CVector(vec.x * scalar, vec.y * scalar, vec.z * scalar);
    }

    // 向量运算
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

    // 向量标准化
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

    // 模长计算
    float len() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // 投影运算
    CVector project(const CVector& n) const {
        float denominator = n.len() * n.len();
        if (denominator == 0) {
            throw std::invalid_argument("Projection onto zero vector");
        }
        float scalar = this->dotMul(n) / denominator;
        return n * scalar;
    }

    // 向量夹角（弧度）
    float angleWith(const CVector& other) const {
        float lengthsProduct = len() * other.len();
        if (lengthsProduct == 0) {
            throw std::invalid_argument("Angle with zero vector");
        }
        float cosTheta = dotMul(other) / lengthsProduct;
        cosTheta = std::max(-1.0f, std::min(1.0f, cosTheta));
        return std::acos(cosTheta);
    }

    // 计算距离
    float distanceTo(const CVector& other) const {
        return (*this - other).len();
    }

    // 转换为欧拉角
    CEuler ToEuler() const {
        CEuler euler;
        CVector dir = Normalized();
        // 俯仰角（绕X轴）
        euler.p = std::asin(dir.y) * 180.0f / M_PI;
        // 航向角（绕Y轴）
        if (std::fabs(dir.x) < 1e-6f && std::fabs(dir.z) < 1e-6f) {
            euler.h = 0.0f;
        }
        else {
            euler.h = -std::atan2f(dir.x, -dir.z) * 180.0f / M_PI;
        }
        // 横滚角设为0
        euler.b = 0.0f;
        euler.Normal(); // 规范化角度
        return euler;
    }

    // 类型转换（隐式转换为float数组指针）
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