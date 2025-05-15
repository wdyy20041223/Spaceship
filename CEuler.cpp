#include "CEuler.h"
#include <cmath>  // ��ѧ��������
#include "CVector.h"
#include <stdlib.h>
#define M_PI 3.14159265358979323846f

CEuler::CEuler() : h(0.0f), p(0.0f), b(0.0f) {};

CEuler::CEuler(float h, float p, float b) : h(h), p(p), b(b) {};

void CEuler::Normal() {
    // �������������Ƕȹ淶�� [-180, 180)
    auto wrapAngle = [](float angle) -> float {
        angle = std::fmod(angle + 180.0f, 360.0f);
        if (angle < 0) angle += 360.0f; // ��֤����� [0, 360)
        return angle - 180.0f;          // ����� [-180, 180)
        };

    // Step 1: �淶 h �� b �� [-180, 180)
    h = wrapAngle(h);
    b = wrapAngle(b);

    // Step 2: �������� p
    p = wrapAngle(p); // �ȹ淶�� [-180, 180)

    // �����������Ƶ� [-90��, 90��]
    if (p > 90.0f) {
        p = 180.0f - p;    // �� p ӳ�䵽 (90��, 180��] �� [0��, 90��)
        h += 180.0f;       // ���������
        b += 180.0f;       // ���������
    }
    else if (p < -90.0f) {
        p = -180.0f - p;   // �� p ӳ�䵽 [-180��, -90��) �� (-90��, 0��]
        h += 180.0f;
        b += 180.0f;
    }

    // Step 3: �ٴι淶 h �� b
    h = wrapAngle(h);
    b = wrapAngle(b);

    // Step 4: ����������p �� ��90��ʱ�ϲ�h��b��
    if (std::abs(std::abs(p) - 90.0f) < 1e-4f) {
        h = wrapAngle(h + b); // �ϲ�����ǵ������
        b = 0.0f;             // ���������
    }
}

// ת��Ϊ��Ԫ��
CQuaternion CEuler::ToQuaternion() const {
    CEuler normalized = *this;
    normalized.Normal(); // ǿ�ƹ淶��
    const float h_rad = normalized.h * (M_PI / 180.0f) * 0.5f;
    const float p_rad = normalized.p * (M_PI / 180.0f) * 0.5f;
    const float b_rad = normalized.b * (M_PI / 180.0f) * 0.5f;

    // �����������Ǻ���ֵ
    const float cy = cos(h_rad);  // Z�ᣨ����ǣ�
    const float sy = sin(h_rad);
    const float cp = cos(p_rad);  // Y�ᣨ�����ǣ�
    const float sp = sin(p_rad);
    const float cb = cos(b_rad);  // X�ᣨ����ǣ�
    const float sb = sin(b_rad);

    // �� Z-Y-X ˳�������Ԫ����q = q_roll * q_pitch * q_yaw��
    return CQuaternion{
    -(cy * cp * cb + sy * sp * sb),  // w
    -(cy * sp * cb + sy * cp * sb),  // x�����������ȷ�
    -(sy * cp * cb - cy * sp * sb),  // y�����������ȷ�
    -(cy * cp * sb - sy * sp * cb)   // z
    };
}

// ת��Ϊ��ת����
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

    // �������ţ�����OpenGL��-Z��ʼ����
    double x = -cosP * sinH;  // xȡ��
    double y = sinP;
    double z = -cosP * cosH;  // zȡ��

    return CVector(x, y, z);
}
void CEuler::output() {
    printf("%f %f %f\n", this->h, this->p, this->b);
}