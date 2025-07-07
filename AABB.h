
#pragma once
#include "CVector.h"
#include <vector>         // 添加 vector 头文件
#include <string>         // 添加 string 头文件

struct AABB {
    CVector min;
    CVector max;
    std::string partName;
    CMatrix worldTransform; // 添加世界变换矩阵
    CVector pos[8];
    // 添加构造函数
    AABB() = default;
    AABB(const std::string& name, const CVector& minVec, const CVector& maxVec, const CMatrix& transform)
        : partName(name), min(minVec), max(maxVec), worldTransform(transform) {
        for (int i = 0; i < 8; i++) {
            pos[i] = CVector(1, 1, 1);
        }
    }
    // 添加计算局部包围盒的方法
    void CalculateLocalBox(const CVector& minVec, const CVector& maxVec) {
        min = minVec;
        max = maxVec;

        // 计算8个顶点
        pos[0] = min;
        pos[1] = CVector(min.x, min.y, max.z);
        pos[2] = CVector(min.x, max.y, min.z);
        pos[3] = CVector(min.x, max.y, max.z);
        pos[4] = CVector(max.x, min.y, min.z);
        pos[5] = CVector(max.x, min.y, max.z);
        pos[6] = CVector(max.x, max.y, min.z);
        pos[7] = max;
    }
    
};

struct cinfo {
    std::string shipPart;
    std::string astroPart;
    CVector collisionPoint;
};
extern cinfo cInfo[2];


