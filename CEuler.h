#pragma once
#ifndef CEULER_H   
#define CEULER_H  

#include "CQuaternion.h"  // CQuaternion �ǳ�Ա��������ֵ������������
#include "CMatrix.h"      // CMatrix �ǳ�Ա��������ֵ������������

class CVector;

class CEuler {
public:
    float h, p, b;  // �����(yaw)��������(pitch)�������(roll)

    CEuler();
    CEuler(float h, float p, float b);
    // ��Ա��������
    void Normal();
    void output();
    CQuaternion ToQuaternion() const;
    CMatrix ToCMatrix() const;
    CVector ToCVector() const;
};


#endif // CEULER_H