#include "CEuler.h"
#include <cmath>  // 数学函数依赖
#include "CVector.h"
#include <stdlib.h>
#define M_PI 3.14159265358979323846f

CEuler::CEuler() : h(0.0f), p(0.0f), b(0.0f) {};

CEuler::CEuler(float h, float p, float b) : h(h), p(p), b(b) {};

void CEuler::Normal() {
    // 辅助函数：将角度规范到 [-180, 180)
    auto wrapAngle = [](float angle) -> float {
        angle = std::fmod(angle + 180.0f, 360.0f);
        if (angle < 0) angle += 360.0f; // 保证结果在 [0, 360)
        return angle - 180.0f;          // 结果在 [-180, 180)
        };

    // Step 1: 规范 h 和 b 到 [-180, 180)
    h = wrapAngle(h);
    b = wrapAngle(b);

    // Step 2: 处理俯仰角 p
    p = wrapAngle(p); // 先规范到 [-180, 180)

    // 将俯仰角限制到 [-90°, 90°]
    if (p > 90.0f) {
        p = 180.0f - p;    // 将 p 映射到 (90°, 180°] → [0°, 90°)
        h += 180.0f;       // 调整航向角
        b += 180.0f;       // 调整横滚角
    }
    else if (p < -90.0f) {
        p = -180.0f - p;   // 将 p 映射到 [-180°, -90°) → (-90°, 0°]
        h += 180.0f;
        b += 180.0f;
    }

    // Step 3: 再次规范 h 和 b
    h = wrapAngle(h);
    b = wrapAngle(b);

    // Step 4: 万向锁处理（p ≈ ±90°时合并h和b）
    if (std::abs(std::abs(p) - 90.0f) < 1e-4f) {
        h = wrapAngle(h + b); // 合并横滚角到航向角
        b = 0.0f;             // 横滚角清零
    }
}

// 转换为四元数
CQuaternion CEuler::ToQuaternion() const {
    CEuler normalized = *this;
    normalized.Normal(); // 强制规范化
    const float h_rad = normalized.h * (M_PI / 180.0f) * 0.5f;
    const float p_rad = normalized.p * (M_PI / 180.0f) * 0.5f;
    const float b_rad = normalized.b * (M_PI / 180.0f) * 0.5f;

    // 计算各轴的三角函数值
    const float cy = cos(h_rad);  // Z轴（航向角）
    const float sy = sin(h_rad);
    const float cp = cos(p_rad);  // Y轴（俯仰角）
    const float sp = sin(p_rad);
    const float cb = cos(b_rad);  // X轴（横滚角）
    const float sb = sin(b_rad);

    // 按 Z-Y-X 顺序组合四元数（q = q_roll * q_pitch * q_yaw）
    return CQuaternion{
    -(cy * cp * cb + sy * sp * sb),  // w
    -(cy * sp * cb + sy * cp * sb),  // x（修正后的正确项）
    -(sy * cp * cb - cy * sp * sb),  // y（修正后的正确项）
    -(cy * cp * sb - sy * sp * cb)   // z
    };
}

// 转换为旋转矩阵
CMatrix CEuler::ToCMatrix() const {
    CMatrix r1, r2, r3, r4;
    r1.SetRotate(this->h, CVector(0, 1, 0));
    r2.SetRotate(this->p, CVector(1, 0, 0));
    r3.SetRotate(this->b, CVector(0, 0, 1));
    r4 = r1 * r2 * r3;
    return r4;
}

CVector CEuler::ToCVector() const {
    double radH = h * M_PI / 180.0;
    double radP = p * M_PI / 180.0;

    double cosP = cos(radP);
    double sinP = sin(radP);

    double sinH = sin(radH);
    double cosH = cos(radH);

    // 修正符号，考虑OpenGL的-Z初始方向
    double x = -cosP * sinH;  // x取反
    double y = sinP;
    double z = -cosP * cosH;  // z取反

    return CVector(x, y, z);
}
void CEuler::output() {
    printf("%f %f %f\n", this->h, this->p, this->b);
}