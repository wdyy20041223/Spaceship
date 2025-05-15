#pragma once
#include "CVector.h"
#include "CEuler.h"

struct Astronaut {
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
};
void initAstronaut();
void drawAstronaut();