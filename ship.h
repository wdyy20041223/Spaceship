

#pragma once
class CQuaternion;
class CVector;

// ������Ҫ��ͷ�ļ�
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

    CVector lightPosition;   // �������۹��λ��
    CVector lightDirection; // �������۹�Ʒ���

    GLuint bodyTexture;    // ����������
    GLuint seatTexture;     // ��������
    GLuint engineTexture;  // ��������
    GLuint wingTexture;    // ��������
    GLuint glassTexture;   // ��������
    GLuint panelTexture;   // �Ǳ��
    GLuint envMapTexture;  // ������ͼ
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
