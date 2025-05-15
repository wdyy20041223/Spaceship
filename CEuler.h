#pragma once
#ifndef CEULER_H   
#define CEULER_H  

#include "CQuaternion.h"  // CQuaternion 是成员函数返回值，需完整定义
#include "CMatrix.h"      // CMatrix 是成员函数返回值，需完整定义

class CVector;

class CEuler {
public:
    float h, p, b;  // 航向角(yaw)、俯仰角(pitch)、横滚角(roll)

    CEuler();
    CEuler(float h, float p, float b);
    // 成员函数声明
    void Normal();
    void output();
    CQuaternion ToQuaternion() const;
    CMatrix ToCMatrix() const;
    CVector ToCVector() const;
};


#endif // CEULER_H