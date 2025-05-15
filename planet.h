#pragma once
#ifndef PLANET_H   
#define PLANET_H  

#include "CVector.h" 
class CMatrix;

typedef struct ball {
    char name[10];
    CVector centerPlace;//��������
    CVector pointPlace[16][31];
    CVector color;
    CVector speed;
    CVector deltaPlace;
    CVector rotationArix;
    CVector orbitCenter;//��ת����
    float r;
    int slices;
    float rotationSpeed;   // ��ת�ٶ�
    float rotationAngle; //��ת�Ƕ�
    float orbitAngle;//��ת�Ƕ�
    float orbitRadius;//��ת�뾶
    float orbitSpeed;
    CVector orbitPoints[361];  // �洢360���켣�㣨һȦ��
    int index = 0;        // ��ǰ�켣������
    int pointNum;//�켣������
    int startNum;//�켣�㿪ʼλ��
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


extern ball* selectedPlanet; // ��ǰѡ�е�����

extern ball planet[8];

#endif
