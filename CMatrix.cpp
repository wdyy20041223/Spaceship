
#include "CMatrix.h" 
#include "base.h"
#include "CVector.h"
#include "CEuler.h"
#include "CQuaternion.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif



// ������ת��Ϊ��Ԫ��
CQuaternion CMatrix::ToQuaternion() const {
    float m00 = this->m00, m01 = this->m01, m02 = this->m02;
    float m10 = this->m10, m11 = this->m11, m12 = this->m12;
    float m20 = this->m20, m21 = this->m21, m22 = this->m22;
    float trace = m00 + m11 + m22; // ����ļ��������ȶ����жϣ�
    float qw, qx, qy, qz;
    if (trace > 0) {
        // ����������Ԫ��ʵ��
        float s = 0.5f / std::sqrt(trace + 1.0f);
        qw = 0.25f / s;
        qx = (m21 - m12) * s;
        qy = (m02 - m20) * s;
        qz = (m10 - m01) * s;
    }
    else {
        // �������Խ���Ԫ��ѡ����㷽ʽ
        if (m00 > m11 && m00 > m22) {
            float s = 2.0f * std::sqrt(1.0f + m00 - m11 - m22);
            qw = (m21 - m12) / s;
            qx = 0.25f * s;
            qy = (m01 + m10) / s;
            qz = (m02 + m20) / s;
        }
        else if (m11 > m22) {
            float s = 2.0f * std::sqrt(1.0f + m11 - m00 - m22);
            qw = (m02 - m20) / s;
            qx = (m01 + m10) / s;
            qy = 0.25f * s;
            qz = (m12 + m21) / s;
        }
        else {
            float s = 2.0f * std::sqrt(1.0f + m22 - m00 - m11);
            qw = (m10 - m01) / s;
            qx = (m02 + m20) / s;
            qy = (m12 + m21) / s;
            qz = 0.25f * s;
        }
    }
    CQuaternion quat(qw, qx, qy, qz);
    quat.Normalize(); // ��λ����Ԫ��
    return quat;
}

CEuler CMatrix::ToEuler() const {
    CEuler euler;
    CMatrix q = *this;
    float sinp = -q.m12;
    euler.p = std::asin(sinp);
    const float epsilon = 1e-6f;  // ��ֹ�������С��
    const float threshold = 0.9999f;  // cos(89.99��) �� 0.9999
    if (std::abs(sinp) > threshold) {// ������
        euler.h = std::atan2(-q.m20,q.m00);
        euler.b = 0.0f;
    }
    else {
        euler.h = std::atan2(q.m02,q.m22);
        euler.b = std::atan2(q.m10, q.m11);
    }
    const float radToDeg = 180.0f / static_cast<float>(M_PI);
    euler.h *= radToDeg;
    euler.p *= radToDeg;
    euler.b *= radToDeg;
    euler.Normal();
    return euler;
}

void CMatrix::Orthogonalize() {
    CVector X(m00, m10, m20);
    CVector Y(m01, m11, m21);
    CVector Z(m02, m12, m22);

    X.Normalize();

    // ����Gram-Schmidt
    Y = Y - Y.project(X);
    Y.Normalize();

    Z = Z - Z.project(X) - Z.project(Y);
    Z.Normalize();

    m00 = X.x; m10 = X.y; m20 = X.z;
    m01 = Y.x; m11 = Y.y; m21 = Y.z;
    m02 = Z.x; m12 = Z.y; m22 = Z.z;
}

void CMatrix::Set(float* p) {
    float* data = (float*)this; // ͨ��ת���������ȡָ��
    for (int i = 0; i < 16; i++) {
        data[i] = p[i];
    }
}
CMatrix& CMatrix::operator=(const CMatrix& p) { //����ֵ
    m00 = p.m00; m10 = p.m10; m20 = p.m20; m30 = p.m30;
    m01 = p.m01; m11 = p.m11; m21 = p.m21; m31 = p.m31;
    m02 = p.m02; m12 = p.m12; m22 = p.m22; m32 = p.m32;
    m03 = p.m03; m13 = p.m13; m23 = p.m23; m33 = p.m33;
    return *this;
}

CMatrix CMatrix::operator*(float d) {//��������
    float* data = (float*)this; // ͨ��ת���������ȡָ��
    for (int i = 0; i < 16; i++) {
        data[i] *= d;
    }
    return *this;
}

CMatrix CMatrix::operator*(const CMatrix& p) const {
    CMatrix result;
    float* dataA = (float*)this;
    float* dataB = (float*)&p;
    float* resData = (float*)&result;

    for (int i = 0; i < 4; i++) { // �������
        for (int j = 0; j < 4; j++) { // �������
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += dataA[k * 4 + i] * dataB[j * 4 + k];
            }
            resData[j * 4 + i] = sum; // ����洢����j��i
        }
    }
    return result;
}

CVector CMatrix::vecMul(const CVector& p) const {
    CVector result;
    // ǰ���еĻ������ֱ�������������˺����
    result.x = m00 * p.x + m01 * p.y + m02 * p.z;
    result.y = m10 * p.x + m11 * p.y + m12 * p.z;
    result.z = m20 * p.x + m21 * p.y + m22 * p.z;
    return result;
}

CVector CMatrix::posMul(const CVector& p) const {
    CVector result;
    // ǰ���л�������������ˣ����ϵ�����ƽ�Ʒ���
    result.x = m00 * p.x + m01 * p.y + m02 * p.z + m03;
    result.y = m10 * p.x + m11 * p.y + m12 * p.z + m13;
    result.z = m20 * p.x + m21 * p.y + m22 * p.z + m23;
    return result;
}

CMatrix CMatrix::GetInverse() const {
    CMatrix invMat(*this); // ���Ƶ�ǰ����
    float det = invMat.Inverse();
    return (det != 0) ? invMat : CMatrix(); // ����������λ����
}

float CMatrix::Inverse() {
    float* m = (float*)this;
    double temp[4][4];
    double inv[4][4] = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
    double det = 1.0;
    for (int i = 0; i < 4; ++i) {// ����ԭ������ʱ����
        for (int j = 0; j < 4; ++j) {
            temp[i][j] = m[4*i+j];
        }
    }
    for (int col = 0; col < 4; ++col) {// ѡ��Ԫ
        int pivot = col;
        double maxVal = std::fabs(temp[col][col]);
        for (int row = col + 1; row < 4; ++row) {
            if (std::fabs(temp[row][col]) > maxVal) {
                maxVal = std::fabs(temp[row][col]);
                pivot = row;
            }
        }
        if (maxVal < 1e-10) return 0.0f;       
        if (pivot != col) {// �н���
            for (int j = 0; j < 4; ++j) {
                std::swap(temp[col][j], temp[pivot][j]);
                std::swap(inv[col][j], inv[pivot][j]);
            }
            det *= -1;
        }       
        double divisor = temp[col][col];// ��һ����Ԫ��
        for (int j = 0; j < 4; ++j) {
            temp[col][j] /= divisor;
            inv[col][j] /= divisor;
        }
        det *= divisor; // ����ʽ���Գ���        
        for (int row = 0; row < 4; ++row) {// ��Ԫ
            if (row != col) {
                double factor = temp[row][col];
                for (int j = 0; j < 4; ++j) {
                    temp[row][j] -= factor * temp[col][j];
                    inv[row][j] -= factor * inv[col][j];
                }
            }
        }
    }  
    for (int i = 0; i < 4; ++i) {// ���µ�ǰ����Ϊ�����
        for (int j = 0; j < 4; ++j) {
            m[4 * i + j] = inv[i][j];
        }
    }
    return static_cast<float>(det);
}

void CMatrix::SetRotate(float theta, CVector axis) {
    theta = theta * 3.14159265358979323846 / 180;
    // ��׼����ת��
    axis.Normalize();
    CVector normAxis = axis * (1.0f / axis.len());
    float ux = normAxis.x;
    float uy = normAxis.y;
    float uz = normAxis.z;

    float cosTheta = cosf(theta);
    float sinTheta = sinf(theta);
    float oneMinusCos = 1.0f - cosTheta;

    m00 = 1.0f; m10 = 0.0f; m20 = 0.0f; m30 = 0.0f;// ��ʼ����λ����
    m01 = 0.0f; m11 = 1.0f; m21 = 0.0f; m31 = 0.0f;
    m02 = 0.0f; m12 = 0.0f; m22 = 1.0f; m32 = 0.0f;
    m03 = 0.0f; m13 = 0.0f; m23 = 0.0f; m33 = 1.0f;

    m00 = cosTheta + ux * ux * oneMinusCos;// ������ת����Ԫ�أ������ȣ�
    m10 = ux * uy * oneMinusCos + uz * sinTheta;
    m20 = ux * uz * oneMinusCos - uy * sinTheta;

    m01 = ux * uy * oneMinusCos - uz * sinTheta;
    m11 = cosTheta + uy * uy * oneMinusCos;
    m21 = uy * uz * oneMinusCos + ux * sinTheta;

    m02 = ux * uz * oneMinusCos + uy * sinTheta;
    m12 = uy * uz * oneMinusCos - ux * sinTheta;
    m22 = cosTheta + uz * uz * oneMinusCos;
}

void CMatrix::SetTrans(CVector trans) {
    // ��ʼ����λ��������ƽ�Ʒ���
    m00 = 1.0f; m10 = 0.0f; m20 = 0.0f; m30 = 0.0f;
    m01 = 0.0f; m11 = 1.0f; m21 = 0.0f; m31 = 0.0f;
    m02 = 0.0f; m12 = 0.0f; m22 = 1.0f; m32 = 0.0f;
    m03 = trans.x; m13 = trans.y; m23 = trans.z; m33 = 1.0f;
}

void CMatrix::SetScale(CVector scale) {
    // �������ž��󣨶Խ���Ϊ�������ӣ�
    m00 = scale.x; m10 = 0.0f;   m20 = 0.0f;   m30 = 0.0f;
    m01 = 0.0f;   m11 = scale.y; m21 = 0.0f;   m31 = 0.0f;
    m02 = 0.0f;   m12 = 0.0f;   m22 = scale.z; m32 = 0.0f;
    m03 = 0.0f;   m13 = 0.0f;   m23 = 0.0f;   m33 = 1.0f;
}

CMatrix::operator float* () { 
    return &m00;
}


void CMatrix::output() {
    float* data = (float*)this; 
    for (int i = 0; i < 16; i++) {
        printf("%f ",data[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
    }
}

// ����const�汾��ת�������
CMatrix::operator const float* () const {
    return &m00;
}

