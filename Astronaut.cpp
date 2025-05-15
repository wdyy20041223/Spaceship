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
    astronaut.angleStep = 1.8f;   // Ĭ��ת��Ƕ�����
    astronaut.speedLen = 0.001f;    // ��ʼ�ٶ�
    astronaut.leftRightAngle = 0.0f; // ��ʼ����Ƕ�
    astronaut.position = CVector(0, 0, 0); // ��ʼλ�ã�Y=1ģ�����߶ȣ�
    astronaut.direction = CVector(0, 0, 1);
    astronaut.cameraDistLen = 0.3;
    ControlingGlobal = true;
}

void drawAstronaut() {

    float a = 0.03;

    glPushMatrix();

    // �ɴ������任��λ�� + ��ת��
    transMat1.SetTrans(myShip.position);
    rotateMat1.SetRotate(myShip.leftRightAngle, CVector(0, 1, 0));
    rotateMat2.SetRotate(-myShip.upDownAngle, CVector(1, 0, 0));
    CMatrix rotateMat3;
    rotateMat3.SetRotate(myShip.rollAngle, CVector(0,0,1));

    // Ӧ�÷ɴ��任�󣬵���̫���˾ֲ�λ�ú�������ת/����
    finalMat = transMat1 * rotateMat1 * rotateMat2 * rotateMat3;
    transMat1.SetTrans(astronaut.position);
    rotateMat1.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    scaleMat1.SetScale(CVector(0.2 * a, 0.2 * a, 0.2 * a));
    finalMat = finalMat * transMat1 * rotateMat1 * scaleMat1;

    glMultMatrixf(finalMat);

    CMatrix localRotMat;
    localRotMat.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    CVector localDir = localRotMat.vecMul(CVector(0, 0, 1)); // ��ʼǰ����ΪZ��
    astronaut.direction = localDir.Normalized();
    
    astronaut.upDirection = CVector(
        finalMat.m01,  // Y��X��������0�е�1�У�
        finalMat.m11,  // Y��Y��������1�е�1�У�
        finalMat.m21   // Y��Z��������2�е�1�У�
    );
    astronaut.upDirection.Normalize();

    CVector headPosition = CVector(
        finalMat.m03 + 2.2 * finalMat.m01,  // X������ƽ��X + 2.2*Y��X����
        finalMat.m13 + 2.2 * finalMat.m11,  // Y������ƽ��Y + 2.2*Y��Y����
        finalMat.m23 + 2.2 * finalMat.m21   // Z������ƽ��Z + 2.2*Y��Z����
    );
    astronaut.head = headPosition;

    CVector yellow(1.0f, 1.0f, 0.0f);  // RGB��
    CVector blue(0.0f, 0.0f, 1.0f);    // RGB��
    CVector red(1.0f, 0.0f, 0.0f);     // RGB��
    CVector green(0.0f, 1.0f, 0.0f);   // RGB��
    // ͷ��
    glColor3fv(blue);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 2.2, 0));

    glMultMatrixf(transMat1);
    glutSolidSphere(0.3, 12, 6);  
    glPopMatrix();
    glColor3fv(red);
    // ����
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 1.4, 0));
    scaleMat1.SetScale(CVector(0.444, 1, 0.333));
    glMultMatrixf(transMat1 * scaleMat1);
    glutSolidCube(1.0);
    glPopMatrix();
    // �ұ�
    glColor3fv(yellow);
    glPushMatrix();
    transMat1.SetTrans(CVector(0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.1, 0.8, 3, 3);
    glPopMatrix();
    // ���
    glPushMatrix();
    transMat1.SetTrans(CVector(-0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.1, 0.8, 3, 3);
    glPopMatrix();
    // ����
    glColor3fv(green);
    glPushMatrix();
    transMat1.SetTrans(CVector(0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0));  // ��X����ת90�ȣ�ʹ�ȳ���
    rotateMat2.SetRotate(astronaut.rightLegAngle, CVector(1.0, 0.0, 0.0)); 
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);
    glutSolidCone(0.15, 1.0, 3, 3);
    glPopMatrix();

    // ����
    glPushMatrix();
    transMat1.SetTrans(CVector(-0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0)); // ��X����ת90��
    rotateMat2.SetRotate(astronaut.leftLegAngle, CVector(1.0, 0.0, 0.0)); // ��Y��ڶ�
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);
    glutSolidCone(0.15, 1.0, 3, 3);
    glPopMatrix();

    glPopMatrix();

}