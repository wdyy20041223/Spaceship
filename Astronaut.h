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
    float leftLegAngle;  // ×óÍÈ°Ú¶¯½Ç¶È
    float rightLegAngle; // ÓÒÍÈ°Ú¶¯½Ç¶È
};
void initAstronaut();
void drawAstronaut();