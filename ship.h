

#pragma once
class CQuaternion;
class CVector;

// 包含必要的头文件
#include "planet.h"
#include "CVector.h"
#include "CQuaternion.h"

struct ship {
    CQuaternion orientation;
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

    CVector lightPosition;   // 新增：聚光灯位置
    CVector lightDirection; // 新增：聚光灯方向

    GLuint bodyTexture;    // 船体主纹理
    GLuint seatTexture;     // 座椅纹理
    GLuint engineTexture;  // 引擎纹理
    GLuint wingTexture;    // 机翼纹理
    GLuint glassTexture;   // 玻璃材质
    GLuint panelTexture;   // 仪表板
    GLuint envMapTexture;  // 环境贴图
    GLuint floorTexture;
};
CVector getForwardDirection(const CQuaternion& orientation);
CVector getUpDirection(const CQuaternion& orientation);
CVector getRightDirection(const CQuaternion& orientation);

void autoShip();
void initShip();
void shipMove();
CVector getShipDir(ship myShip);
void drawShip();
void drawAxis();
void drawSeat(float len, float pos1, float pos2);

void ShipYaw(float angle);
void ShipPitch(float angle);
void ShipRoll(float angle);
void DrawWing(bool isRightWing);

#pragma once
