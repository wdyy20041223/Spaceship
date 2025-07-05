

#pragma once
class CQuaternion;
class CVector;

// ������Ҫ��ͷ�ļ�
#include "planet.h"
#include "CVector.h"
#include "CQuaternion.h"
#include <vector>         // ��� vector ͷ�ļ�
#include <string>         // ��� string ͷ�ļ�

#include "AABB.h"



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

    std::vector<AABB> collisionBoxes; // �洢��Χ����Ϣ
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
void drawSeat(float len, float pos1, float pos2, const CMatrix& shipTransform);

void ShipYaw(float angle);
void ShipPitch(float angle);
void ShipRoll(float angle);
void DrawWing(bool isRightWing, const CMatrix& shipTransform);



#pragma once
