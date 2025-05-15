

#pragma once
#include "planet.h"
#include "CVector.h"

typedef struct ship {
    CVector position;
    CVector speed;
    CVector direction;
    float upDownAngle;
    float leftRightAngle;
    float rollAngle;
    float angleStep;
    float speedLen;
    float speedTempLen;
    float speedStep;
    bool autoPilot;
    ball* targetBall;
};

void autoShip();
void initShip();
void shipMove();
CVector getShipDir(ship myShip);
void drawShip();
void drawAxis();

#pragma once
