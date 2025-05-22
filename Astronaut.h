#pragma once
#include "CVector.h"
#include "CEuler.h"
#include <GL/glew.h>      // OpenGL��չ

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

    GLuint headTexture;
    GLuint bodyTexture;
    GLuint armTexture;
    GLuint legTexture;
};
void initAstronaut();
void drawAstronaut();