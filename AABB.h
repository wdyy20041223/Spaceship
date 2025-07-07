
#pragma once
#include "CVector.h"
#include <vector>         // ��� vector ͷ�ļ�
#include <string>         // ��� string ͷ�ļ�

struct AABB {
    CVector min;
    CVector max;
    std::string partName;
    CMatrix worldTransform; // �������任����
    CVector pos[8];
    // ��ӹ��캯��
    AABB() = default;
    AABB(const std::string& name, const CVector& minVec, const CVector& maxVec, const CMatrix& transform)
        : partName(name), min(minVec), max(maxVec), worldTransform(transform) {
        for (int i = 0; i < 8; i++) {
            pos[i] = CVector(1, 1, 1);
        }
    }
    // ��Ӽ���ֲ���Χ�еķ���
    void CalculateLocalBox(const CVector& minVec, const CVector& maxVec) {
        min = minVec;
        max = maxVec;

        // ����8������
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


