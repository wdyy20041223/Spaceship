#pragma once
#ifndef CQUATERNION_H
#define CQUATERNION_H

class CVector;
class CMatrix;
class CEuler;

class CQuaternion {
public:
    float w, x, y, z;
    // ���캯��
    CQuaternion() : w(1), x(0), y(0), z(0) {}
    CQuaternion(float w, float x, float y, float z);

    // ���ķ�������
    void Set(float fw, float fx, float fy, float fz);
    void SetAngle(float angle, CVector axis);
    bool Normalize();
    void GetAngle(float& angle, CVector& axis);
    CQuaternion Slerp(const CQuaternion& Vend, float t) const;

    // ���������
    CQuaternion& operator=(const CQuaternion& p);
    CQuaternion operator+(const CQuaternion& p) const;
    CQuaternion operator-(const CQuaternion& p) const;
    CQuaternion operator*(float data) const;
    CVector operator*(const CVector& v) const;
    friend CQuaternion operator*(float scalar, const CQuaternion& q);
    CQuaternion operator*(const CQuaternion& p) const;
    operator float* () { return &w; }

    // ���ߺ���
    float dotMul(const CQuaternion& p) const;
    float len() const;
    CQuaternion& Inverse();
    CQuaternion GetInverse() const;
    CQuaternion Div(const CQuaternion& b) const;
    void Slerp(const CQuaternion& Vend, int n, float* t, CQuaternion* Result) const;
    CEuler ToEuler() const;   // ת��Ϊŷ����
    CMatrix ToMatrix() const; // ת��Ϊ��ת����
    
    void output();

    CQuaternion CreateReverseQuaternion();
};

#endif // CQUATERNION_H