#include "CQuaternion.h"
#include <cmath>
#include "CEuler.h"
#include "CMatrix.h"
#include "CVector.h"
#include <algorithm>


#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// 球面线性插值
CQuaternion CQuaternion::Slerp(const CQuaternion& end, float t) const {
    CQuaternion start = *this;
    CQuaternion qEnd = end;

    // 单位化输入四元数
    if (!start.Normalize() || !qEnd.Normalize()) {
        return CQuaternion(1, 0, 0, 0); // 返回默认单位四元数
    }
    
    // 计算点积，判断是否需要反转
    float dot = start.w * qEnd.w + start.x * qEnd.x + start.y * qEnd.y + start.z * qEnd.z;

    if (dot < 0.0f) {
        // 反转 qEnd，保证插值走最短路径
        qEnd.w = -qEnd.w;
        qEnd.x = -qEnd.x;
        qEnd.y = -qEnd.y;
        qEnd.z = -qEnd.z;
        dot = -dot;
    }

    // 当两四元数非常接近时，使用线性插值避免数值不稳定
    if (dot > 0.9995f) {
        // 线性插值
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
    q.Normalize();  // 确保四元数单位化
    float w = q.w, x = q.x, y = q.y, z = q.z;
    float sinp = 2 * (w * x - y * z);
    euler.p = std::asin(sinp);
    const float epsilon = 1e-6f;  // 防止除以零的小量
    const float threshold = 0.9999f;  // cos(89.99°) ≈ 0.9999
    if (std::abs(sinp) > threshold) {
        // 万向锁情况：合并heading和bank
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

    // 列主序公式计算
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

// 带参数的构造函数
CQuaternion::CQuaternion(float w, float x, float y, float z)
    : w(w), x(x), y(y), z(z) {
}

// 设置四元数值
void CQuaternion::Set(float fw, float fx, float fy, float fz) {
    w = fw; x = fx; y = fy; z = fz;
}

// 设置旋转四元数（自动单位化）
void CQuaternion::SetAngle(float angle, CVector axis) {
    const float rad = angle * 0.5f * static_cast<float>(M_PI) / 180.0f;
    axis.Normalize();
    w = std::cos(rad);
    const float s = std::sin(rad);
    x = axis.x * s;
    y = axis.y * s;
    z = axis.z * s;
    this->Normalize();  // 确保生成单位四元数
}

// 单位化
bool CQuaternion::Normalize() {
    const float l = len();
    if (l < 1e-6) return false;
    w /= l; x /= l; y /= l; z /= l;
    return true;
}

void CQuaternion::GetAngle(float& angle, CVector& axis) {
    CQuaternion tmp(*this);
    if (tmp.Normalize() == false) {  // 强制单位化
        angle = 0;
        axis.Set(1, 0, 0);
        return;
    }

    const float theta = 2 * std::acos(tmp.w);
    angle = theta * 180.0f / static_cast<float>(M_PI);

    const float s = std::sqrt(1 - tmp.w * tmp.w);
    if (s < 1e-6) {
        axis.Set(1, 0, 0); // 接近零时默认绕 X 轴
    } else {
        axis.Set(tmp.x / s, tmp.y / s, tmp.z / s);
        axis.Normalize(); // 确保轴单位化
    }

    // 将角度规范到 [-180°, 180°]
    if (angle > 180.0f) {
        angle -= 360.0f;   // 转换为负角度
    }
}

// 运算符重载
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
    return q * scalar; // 直接复用成员函数的实现
}

CQuaternion CQuaternion::operator*(const CQuaternion& p) const {
    return CQuaternion(
        w * p.w - x * p.x - y * p.y - z * p.z,
        w * p.x + x * p.w + y * p.z - z * p.y,
        w * p.y - x * p.z + y * p.w + z * p.x,
        w * p.z + x * p.y - y * p.x + z * p.w
    );
}

// 点积
float CQuaternion::dotMul(const CQuaternion& p) const {
    return w * p.w + x * p.x + y * p.y + z * p.z;
}

// 四元数长度
float CQuaternion::len() const {
    return std::sqrt(w * w + x * x + y * y + z * z);
}

// 求逆（原地）
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

// 求逆（返回新对象）
CQuaternion CQuaternion::GetInverse() const {
    float norm = w * w + x * x + y * y + z * z;
    return CQuaternion(w / norm, -x / norm, -y / norm, -z / norm);
}

// 四元数除法
CQuaternion CQuaternion::Div(const CQuaternion& b) const {
    CQuaternion a = (*this);
    return b * a.Inverse();
}

// 批量插值
void CQuaternion::Slerp(const CQuaternion& Vend, int n, float* t, CQuaternion* Result) const {
    for (int i = 0; i < n; ++i) {
        Result[i] = this->Slerp(Vend, t[i]);
    }
}

void CQuaternion::output() {
    printf("%f %f %f %f\n", this->w, this->x, this->y, this->z);
}

CVector CQuaternion::operator*(const CVector& v) const {
    // 将向量转换为纯四元数 (w=0)
    CQuaternion vecQuat(0, v.x, v.y, v.z);

    // 计算旋转后的四元数: q * v * q^-1
    CQuaternion result = (*this) * vecQuat * this->GetInverse();

    // 返回旋转后的向量部分
    return CVector(result.x, result.y, result.z);
}

