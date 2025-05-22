#pragma once
#include "CVector.h"
#include "CEuler.h"
#include <GL/glew.h>      // OpenGL扩展

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
    float leftLegAngle;  // 左腿摆动角度
    float rightLegAngle; // 右腿摆动角度

    GLuint headTexture;
    GLuint bodyTexture;
    GLuint armTexture;
    GLuint legTexture;
};
void initAstronaut();
void drawAstronaut();