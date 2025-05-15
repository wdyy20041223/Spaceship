#pragma once
#ifndef PLANET_H   
#define PLANET_H  

#include "CVector.h" 
class CMatrix;

typedef struct ball {
    char name[10];
    CVector centerPlace;//行星中心
    CVector pointPlace[16][31];
    CVector color;
    CVector speed;
    CVector deltaPlace;
    CVector rotationArix;
    CVector orbitCenter;//公转中心
    float r;
    int slices;
    float rotationSpeed;   // 自转速度
    float rotationAngle; //自转角度
    float orbitAngle;//公转角度
    float orbitRadius;//公转半径
    float orbitSpeed;
    CVector orbitPoints[361];  // 存储360个轨迹点（一圈）
    int index = 0;        // 当前轨迹点索引
    int pointNum;//轨迹点数量
    int startNum;//轨迹点开始位置
    bool selected;
};

void initPlanet();
void drawRing(ball saturn);
void initBall(ball& ball0);
void planetRotation();
void drawPlanet();
void drawTrack(ball ball0);
void drawBall(ball ball0);
void mouseClick(int button, int state, int x, int y);


extern ball* selectedPlanet; // 当前选中的行星

extern ball planet[8];

#endif
