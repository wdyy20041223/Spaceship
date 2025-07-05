#pragma once
#include "CVector.h"
#include "CEuler.h"
#include <GL/glew.h>      // OpenGL��չ
#include <vector>         // ��� vector ͷ�ļ�
#include <string>         // ��� string ͷ�ļ�

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
    float leftLegAngle;  // ���Ȱڶ��Ƕ�
    float rightLegAngle; // ���Ȱڶ��Ƕ�

    GLuint headTexture;
    GLuint bodyTexture;
    GLuint armTexture;
    GLuint legTexture;

    std::vector<AABB> collisionBoxes; // �洢��Χ����Ϣ
    
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