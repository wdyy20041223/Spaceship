// Astronaut.cpp
#include "Astronaut.h"
#include <GL/glut.h>
#include "CMatrix.h"
#include "ship.h"
#include "camera.h"

extern CMatrix transMat1, transMat2, rotateMat1, rotateMat2, scaleMat1, scaleMat2, finalMat;
extern Astronaut astronaut;
extern ship myShip;
extern Camera astronautCamera;
extern bool ControlingGlobal;

void initAstronaut() {
    astronaut.angleStep = 1.8f;   // 默认转向角度增量
    astronaut.speedLen = 0.001f;    // 初始速度
    astronaut.leftRightAngle = 0.0f; // 初始朝向角度
    astronaut.position = CVector(0, 0, 0); // 初始位置（Y=1模拟地面高度）
    astronaut.direction = CVector(0, 0, 1);
    astronaut.cameraDistLen = 0.3;
    ControlingGlobal = true;
}

void drawAstronaut() {

    float a = 0.03;

    glPushMatrix();

    // 飞船基础变换（位置 + 旋转）
    transMat1.SetTrans(myShip.position);
    rotateMat1.SetRotate(myShip.leftRightAngle, CVector(0, 1, 0));
    rotateMat2.SetRotate(-myShip.upDownAngle, CVector(1, 0, 0));
    CMatrix rotateMat3;
    rotateMat3.SetRotate(myShip.rollAngle, CVector(0,0,1));

    // 应用飞船变换后，叠加太空人局部位置和自身旋转/缩放
    finalMat = transMat1 * rotateMat1 * rotateMat2 * rotateMat3;
    transMat1.SetTrans(astronaut.position);
    rotateMat1.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    scaleMat1.SetScale(CVector(0.2 * a, 0.2 * a, 0.2 * a));
    finalMat = finalMat * transMat1 * rotateMat1 * scaleMat1;

    glMultMatrixf(finalMat);

    CMatrix localRotMat;
    localRotMat.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    CVector localDir = localRotMat.vecMul(CVector(0, 0, 1)); // 初始前方向为Z轴
    astronaut.direction = localDir.Normalized();
    
    astronaut.upDirection = CVector(
        finalMat.m01,  // Y轴X分量（第0行第1列）
        finalMat.m11,  // Y轴Y分量（第1行第1列）
        finalMat.m21   // Y轴Z分量（第2行第1列）
    );
    astronaut.upDirection.Normalize();

    CVector headPosition = CVector(
        finalMat.m03 + 2.2 * finalMat.m01,  // X分量：平移X + 2.2*Y轴X方向
        finalMat.m13 + 2.2 * finalMat.m11,  // Y分量：平移Y + 2.2*Y轴Y方向
        finalMat.m23 + 2.2 * finalMat.m21   // Z分量：平移Z + 2.2*Y轴Z方向
    );
    astronaut.head = headPosition;

    CVector yellow(1.0f, 1.0f, 0.0f);  // RGB黄
    CVector blue(0.0f, 0.0f, 1.0f);    // RGB蓝
    CVector red(1.0f, 0.0f, 0.0f);     // RGB红
    CVector green(0.0f, 1.0f, 0.0f);   // RGB绿
    // 头部
    glColor3fv(blue);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 2.2, 0));

    glMultMatrixf(transMat1);
    glutSolidSphere(0.3, 12, 6);  
    glPopMatrix();
    glColor3fv(red);
    // 躯干
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 1.4, 0));
    scaleMat1.SetScale(CVector(0.444, 1, 0.333));
    glMultMatrixf(transMat1 * scaleMat1);
    glutSolidCube(1.0);
    glPopMatrix();
    // 右臂
    glColor3fv(yellow);
    glPushMatrix();
    transMat1.SetTrans(CVector(0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.1, 0.8, 3, 3);
    glPopMatrix();
    // 左臂
    glPushMatrix();
    transMat1.SetTrans(CVector(-0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.1, 0.8, 3, 3);
    glPopMatrix();
    // 右腿
    glColor3fv(green);
    glPushMatrix();
    transMat1.SetTrans(CVector(0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0));  // 绕X轴旋转90度，使腿朝下
    rotateMat2.SetRotate(astronaut.rightLegAngle, CVector(1.0, 0.0, 0.0)); 
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);
    glutSolidCone(0.15, 1.0, 3, 3);
    glPopMatrix();

    // 左腿
    glPushMatrix();
    transMat1.SetTrans(CVector(-0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0)); // 绕X轴旋转90度
    rotateMat2.SetRotate(astronaut.leftLegAngle, CVector(1.0, 0.0, 0.0)); // 绕Y轴摆动
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);
    glutSolidCone(0.15, 1.0, 3, 3);
    glPopMatrix();

    glPopMatrix();

}