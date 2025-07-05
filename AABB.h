
#pragma once
#include "CVector.h"
#include <vector>         // 添加 vector 头文件
#include <string>         // 添加 string 头文件

struct AABB {
    CVector min;
    CVector max;
    std::string partName;
    CMatrix worldTransform; // 添加世界变换矩阵
    // 添加构造函数
    AABB() = default;
    AABB(const std::string& name, const CVector& minVec, const CVector& maxVec, const CMatrix& transform)
        : partName(name), min(minVec), max(maxVec), worldTransform(transform) {
    }
};


