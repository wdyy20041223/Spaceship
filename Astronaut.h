#pragma once
#include "CVector.h"
#include "CEuler.h"
#include <GL/glew.h>      // OpenGL扩展
#include <vector>         // 添加 vector 头文件
#include <string>         // 添加 string 头文件

#include "AABB.h"


struct Astronaut {
    CQuaternion localRotation;

    CVector finalDir;
    CVector position;
    CVector speed;
    CVector direction;
    CVector upDirection;
    CEuler allAngle;
    float upDownAngle;
    float leftRightAngle;
    float angleStep;
    float speedStep;
    float speedLen;
    float cameraDistLen;
    CVector head;
    float leftLegAngle;  // 左腿摆动角度
    float rightLegAngle; // 右腿摆动角度

    GLuint headTexture;
    GLuint bodyTexture;
    GLuint armTexture;
    GLuint legTexture;

    std::vector<AABB> collisionBoxes; // 存储包围盒信息
    
    bool Headcollised;
    bool Bodycollised;
    bool LeftArmcollised;
    bool RightArmcollised;
    bool LeftLegcollised;
    bool RightLegcollised;
    
};
void initAstronaut();
void drawAstronaut();

void DrawAABB(const AABB& box, char category);
void DrawAABB2(const AABB& box, char category);