#include "CQuaternion.h"
#include <cmath>
#include "CEuler.h"
#include "CMatrix.h"
#include "CVector.h"
#include <algorithm>


#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// �������Բ�ֵ
CQuaternion CQuaternion::Slerp(const CQuaternion& end, float t) const {
    CQuaternion start = *this;
    CQuaternion qEnd = end;

    // ��λ��������Ԫ��
    if (!start.Normalize() || !qEnd.Normalize()) {
        return CQuaternion(1, 0, 0, 0); // ����Ĭ�ϵ�λ��Ԫ��
    }
    
    // ���������ж��Ƿ���Ҫ��ת
    float dot = start.w * qEnd.w + start.x * qEnd.x + start.y * qEnd.y + start.z * qEnd.z;

    if (dot < 0.0f) {
        // ��ת qEnd����֤��ֵ�����·��
        qEnd.w = -qEnd.w;
        qEnd.x = -qEnd.x;
        qEnd.y = -qEnd.y;
        qEnd.z = -qEnd.z;
        dot = -dot;
    }

    // ������Ԫ���ǳ��ӽ�ʱ��ʹ�����Բ�ֵ������ֵ���ȶ�
    if (dot > 0.9995f) {
        // ���Բ�ֵ
        CQuaternion result(
            start.w + t * (qEnd.w - start.w),
            start.x + t * (qEnd.x - start.x),
            start.y + t * (qEnd.y - start.y),
            start.z + t * (qEnd.z - start.z)
        );
        result.Normalize();
        return result;
    }

    CQuaternion result;
    float angle;
    CVector axis;

    result = start.Div(qEnd);
    result.GetAngle(angle, axis);
    result.SetAngle(angle * t, axis);
    result = result * start;
    result.Normalize();
    return result;
}

CEuler CQuaternion::ToEuler() const {
    CEuler euler;
    CQuaternion q = *this;
    q.Normalize();  // ȷ����Ԫ����λ��
    float w = q.w, x = q.x, y = q.y, z = q.z;
    float sinp = 2 * (w * x - y * z);
    euler.p = std::asin(sinp);
    const float epsilon = 1e-6f;  // ��ֹ�������С��
    const float threshold = 0.9999f;  // cos(89.99��) �� 0.9999
    if (std::abs(sinp) > threshold) {
        // ������������ϲ�heading��bank
        euler.h = std::atan2(2 * (w * y - x * z), epsilon + 1 - 2 * (z * z + y * y));
        euler.b = 0.0f;
    }
    else {
        euler.h = std::atan2(2 * (w * y + x * z), 1 - 2 * (x * x + y * y));
        euler.b = std::atan2(2 * (x * y + w * z), 1 - 2 * (z * z + x * x));
    }
    const float radToDeg = 180.0f / static_cast<float>(M_PI);
    euler.h *= radToDeg;
    euler.p *= radToDeg;
    euler.b *= radToDeg;
    euler.Normal();  
    return euler;
}

CMatrix CQuaternion::ToMatrix() const {
    CMatrix mat;
    CQuaternion q = *this;
    q.Normalize();
    float w = q.w, x = q.x, y = q.y, z = q.z;

    // ������ʽ����
    float m00 = 1 - 2 * y * y - 2 * z * z;
    float m01 = 2 * x * y - 2 * w * z;
    float m02 = 2 * x * z + 2 * w * y;

    float m10 = 2 * x * y + 2 * w * z;
    float m11 = 1 - 2 * x * x - 2 * z * z;
    float m12 = 2 * y * z - 2 * w * x;

    float m20 = 2 * x * z - 2 * w * y;
    float m21 = 2 * y * z + 2 * w * x;
    float m22 = 1 - 2 * x * x - 2 * y * y;

    mat.m00 = m00; mat.m01 = m01; mat.m02 = m02; mat.m03 = 0;
    mat.m10 = m10; mat.m11 = m11; mat.m12 = m12; mat.m13 = 0;
    mat.m20 = m20; mat.m21 = m21; mat.m22 = m22; mat.m23 = 0;
    mat.m30 = 0;   mat.m31 = 0;   mat.m32 = 0;    mat.m33 = 1;

    return mat;
}

// �������Ĺ��캯��
CQuaternion::CQuaternion(float w, float x, float y, float z)
    : w(w), x(x), y(y), z(z) {
}

// ������Ԫ��ֵ
void CQuaternion::Set(float fw, float fx, float fy, float fz) {
    w = fw; x = fx; y = fy; z = fz;
}

// ������ת��Ԫ�����Զ���λ����
void CQuaternion::SetAngle(float angle, CVector axis) {
    const float rad = angle * 0.5f * static_cast<float>(M_PI) / 180.0f;
    axis.Normalize();
    w = std::cos(rad);
    const float s = std::sin(rad);
    x = axis.x * s;
    y = axis.y * s;
    z = axis.z * s;
    this->Normalize();  // ȷ�����ɵ�λ��Ԫ��
}

// ��λ��
bool CQuaternion::Normalize() {
    const float l = len();
    if (l < 1e-6) return false;
    w /= l; x /= l; y /= l; z /= l;
    return true;
}

void CQuaternion::GetAngle(float& angle, CVector& axis) {
    CQuaternion tmp(*this);
    if (tmp.Normalize() == false) {  // ǿ�Ƶ�λ��
        angle = 0;
        axis.Set(1, 0, 0);
        return;
    }

    const float theta = 2 * std::acos(tmp.w);
    angle = theta * 180.0f / static_cast<float>(M_PI);

    const float s = std::sqrt(1 - tmp.w * tmp.w);
    if (s < 1e-6) {
        axis.Set(1, 0, 0); // �ӽ���ʱĬ���� X ��
    } else {
        axis.Set(tmp.x / s, tmp.y / s, tmp.z / s);
        axis.Normalize(); // ȷ���ᵥλ��
    }

    // ���Ƕȹ淶�� [-180��, 180��]
    if (angle > 180.0f) {
        angle -= 360.0f;   // ת��Ϊ���Ƕ�
    }
}

// ���������
CQuaternion& CQuaternion::operator=(const CQuaternion& p) {
    w = p.w; x = p.x; y = p.y; z = p.z;
    return *this;
}

CQuaternion CQuaternion::operator+(const CQuaternion& p) const {
    return CQuaternion(w + p.w, x + p.x, y + p.y, z + p.z);
}
CQuaternion CQuaternion::operator-(const CQuaternion& p) const {
    return CQuaternion(
        w - p.w,
        x - p.x,
        y - p.y,
        z - p.z
    );
}
CQuaternion CQuaternion::operator*(float data) const {
    return CQuaternion(w * data, x * data, y * data, z * data);
}
CQuaternion operator*(float scalar, const CQuaternion& q) {
    return q * scalar; // ֱ�Ӹ��ó�Ա������ʵ��
}

CQuaternion CQuaternion::operator*(const CQuaternion& p) const {
    return CQuaternion(
        w * p.w - x * p.x - y * p.y - z * p.z,
        w * p.x + x * p.w + y * p.z - z * p.y,
        w * p.y - x * p.z + y * p.w + z * p.x,
        w * p.z + x * p.y - y * p.x + z * p.w
    );
}

// ���
float CQuaternion::dotMul(const CQuaternion& p) const {
    return w * p.w + x * p.x + y * p.y + z * p.z;
}

// ��Ԫ������
float CQuaternion::len() const {
    return std::sqrt(w * w + x * x + y * y + z * z);
}

// ���棨ԭ�أ�
CQuaternion& CQuaternion::Inverse() {
    float norm = w * w + x * x + y * y + z * z;
    if (norm > 0) {
        float invNorm = 1.0f / norm;
        w *= invNorm;
        x *= -invNorm;
        y *= -invNorm;
        z *= -invNorm;
    }
    return *this;
}

// ���棨�����¶���
CQuaternion CQuaternion::GetInverse() const {
    float norm = w * w + x * x + y * y + z * z;
    return CQuaternion(w / norm, -x / norm, -y / norm, -z / norm);
}

// ��Ԫ������
CQuaternion CQuaternion::Div(const CQuaternion& b) const {
    CQuaternion a = (*this);
    return b * a.Inverse();
}

// ������ֵ
void CQuaternion::Slerp(const CQuaternion& Vend, int n, float* t, CQuaternion* Result) const {
    for (int i = 0; i < n; ++i) {
        Result[i] = this->Slerp(Vend, t[i]);
    }
}

void CQuaternion::output() {
    printf("%f %f %f %f\n", this->w, this->x, this->y, this->z);
}

CVector CQuaternion::operator*(const CVector& v) const {
    // ������ת��Ϊ����Ԫ�� (w=0)
    CQuaternion vecQuat(0, v.x, v.y, v.z);

    // ������ת�����Ԫ��: q * v * q^-1
    CQuaternion result = (*this) * vecQuat * this->GetInverse();

    // ������ת�����������
    return CVector(result.x, result.y, result.z);
}

