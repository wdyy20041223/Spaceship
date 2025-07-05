
#pragma once
#include "CVector.h"
#include <vector>         // ��� vector ͷ�ļ�
#include <string>         // ��� string ͷ�ļ�

struct AABB {
    CVector min;
    CVector max;
    std::string partName;
    CMatrix worldTransform; // �������任����
    // ��ӹ��캯��
    AABB() = default;
    AABB(const std::string& name, const CVector& minVec, const CVector& maxVec, const CMatrix& transform)
        : partName(name), min(minVec), max(maxVec), worldTransform(transform) {
    }
};


