#include "base.h"

class CVector {
public:
    float x, y, z;
    CVector() : x(0), y(0), z(0) {}
    ~CVector() {}
    CVector(float x, float y, float z) : x(x), y(y), z(z) {}
    void Set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
    CVector operator+(CVector& p) {//重载加法
        return CVector(x + p.x, y + p.y, z + p.z);
    }
    CVector operator-(CVector& p) {//重载减法
        return CVector(x - p.x, y - p.y, z - p.z);
    }
    CVector& operator=(const CVector& p) {//重载赋值
        if (this != &p) { x = p.x; y = p.y; z = p.z; }
        return *this;
    }
    bool operator==(const CVector& p){//重载比较
        return (x == p.x) && (y == p.y) && (z == p.z);
    }
    bool operator!=(const CVector& p){//重载比较
        return !(*this == p);
    }
    CVector operator*(float scalar){//重载数乘
        return CVector(x * scalar, y * scalar, z * scalar);
    }
    friend CVector operator*(float scalar,CVector& vec) {
        return vec * scalar;
    }
    float dotMul(const CVector& n){//重载点乘
        return x * n.x + y * n.y + z * n.z;
    }
    CVector crossMul(const CVector& n){//重载叉乘
        return CVector(
            y * n.z - z * n.y,
            z * n.x - x * n.z,
            x * n.y - y * n.x
        );
    }
    void Normalize() {//单位化
        float length = len();
        if (length != 0) {
            x /= length;
            y /= length;
            z /= length;
        }
    }
    float len(){//求模
        return std::sqrt(x * x + y * y + z * z);
    }
    CVector project(CVector& n){//投影
        float scalar = dotMul(n) / (n.len() * n.len());
        return n * scalar;
    }
    operator float* () { return &x; }
};

bool Calculate()
{
    FILE* fp, * fpOut;
    char str[1024];
    fopen_s(&fp, "test.txt", "rt");
    fopen_s(&fpOut, "out.txt", "wt");
    while (!feof(fp))
    {
        fscanf_s(fp, "%s\n", str, 1024);
        if (strcmp(str, "向量相加") == 0)
        {
            CVector v1, v2, vout;
            fscanf_s(fp, "%f,%f,%f %f,%f,%f", &v1.x, &v1.y, &v1.z, &v2.x, &v2.y, &v2.z);
            vout = v1 + v2;
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g \t%g,%g,%g\n", str, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, vout.x, vout.y, vout.z);
        }
        else if (strcmp(str, "向量相减") == 0)
        {
            CVector v1, v2, vout;
            fscanf_s(fp, "%f,%f,%f %f,%f,%f", &v1.x, &v1.y, &v1.z, &v2.x, &v2.y, &v2.z);
            vout = v1 - v2;
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g \t%g,%g,%g\n", str, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, vout.x, vout.y, vout.z);
        }
        else if (strcmp(str, "向量点乘") == 0)
        {
            CVector v1, v2;
            fscanf_s(fp, "%f,%f,%f %f,%f,%f", &v1.x, &v1.y, &v1.z, &v2.x, &v2.y, &v2.z);
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g \t%g\n", str, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v1.dotMul(v2));
        }
        fgets(str, 1024, fp);
    }
    fclose(fp);
    fclose(fpOut);
    return false;
}

typedef struct star {
    CVector place;
    CVector color;
    float size;
};

typedef struct ball {
    CVector place;
    CVector color;
    CVector speed;
    float r;
    int slices;
};

#pragma once
