// Astronaut.cpp
#include "Astronaut.h"
#include <GL/glew.h>      // OpenGL��չ
#include <GL/glut.h>      // GLUT���߰�
#include "CMatrix.h"
#include "ship.h"
#include "camera.h"
#include "planet.h"

extern CMatrix transMat1, transMat2, rotateMat1, rotateMat2, scaleMat1, scaleMat2, finalMat;
extern Astronaut astronaut;
extern ship myShip;
extern Camera astronautCamera;
extern bool ControlingGlobal;

void initAstronaut() {
    astronaut.angleStep = 1.8f;   // Ĭ��ת��Ƕ�����
    astronaut.speedLen = 0.001f;    // ��ʼ�ٶ�
    astronaut.leftRightAngle = 0.0f; // ��ʼ����Ƕ�
    astronaut.position = CVector(0, -0.01, 0); // ��ʼλ�ã�Y=1ģ�����߶ȣ�
    astronaut.direction = CVector(0, 0, 1);
    astronaut.cameraDistLen = 0.3;
    ControlingGlobal = true;

    astronaut.headTexture = LoadTexture("textures/astronaut_head.jpg");
    astronaut.bodyTexture = LoadTexture("textures/astronaut_body.jpg");
    astronaut.armTexture = LoadTexture("textures/astronaut_arm.jpg");
    astronaut.legTexture = LoadTexture("textures/astronaut_leg.jpg");
}

void drawAstronaut() {

    float a = 0.03;

    glPushMatrix();

    // �ɴ������任��λ�� + ��ת��
    transMat1.SetTrans(myShip.position);
    rotateMat1.SetRotate(myShip.leftRightAngle, CVector(0, 1, 0));
    rotateMat2.SetRotate(-myShip.upDownAngle, CVector(1, 0, 0));
    CMatrix rotateMat3;
    rotateMat3.SetRotate(myShip.rollAngle, CVector(0, 0, 1));

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
    // ================== �������� ==================
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // ================== ͷ�������壩==================
    glBindTexture(GL_TEXTURE_2D, astronaut.headTexture);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 2.2, 0));
    glMultMatrixf(transMat1);
    glColor3f(1, 1, 1);

    // ��������ӳ����������
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glutSolidSphere(0.3, 32, 32);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== ���ɣ������壩==================
    glBindTexture(GL_TEXTURE_2D, astronaut.bodyTexture);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 1.4, 0));
    scaleMat1.SetScale(CVector(0.444, 1, 0.333));
    glMultMatrixf(transMat1 * scaleMat1);

    // ���������������ƽ�����
    GLfloat sPlane[] = { 1.0f, 0.0f, 0.0f, 0.0f };  // X�᷽��ϵ����2.25��Ϊ1.0
    GLfloat tPlane[] = { 0.0f, 1.0f, 0.0f, 0.0f };   // Y�᷽��

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlane);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glutSolidCube(1.0);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== �ұۣ�Բ׶�壩==================
    glBindTexture(GL_TEXTURE_2D, astronaut.armTexture);
    glPushMatrix();
    glColor3f(1,1,1);
    transMat1.SetTrans(CVector(0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1);


    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glutSolidCone(0.1, 0.8, 16, 8);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== ��ۣ�Բ׶�壩================== 
    glPushMatrix();
    glColor3f(1, 1, 1); // ������ɫΪ��ɫ
    transMat1.SetTrans(CVector(-0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glutSolidCone(0.1, 0.8, 16, 8);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== ���ȣ�Բ׶�壩==================
    glBindTexture(GL_TEXTURE_2D, astronaut.legTexture);
    glPushMatrix();
    glColor3f(1, 1, 1); // ������ɫΪ��ɫ
    transMat1.SetTrans(CVector(0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0));
    rotateMat2.SetRotate(astronaut.rightLegAngle, CVector(1.0, 0.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

    // ����������������
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glutSolidCone(0.15, 1.0, 16, 8);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== ���ȣ�Բ׶�壩==================
    glPushMatrix();
    glColor3f(1, 1, 1); // ������ɫΪ��ɫ
    transMat1.SetTrans(CVector(-0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0));
    rotateMat2.SetRotate(astronaut.leftLegAngle, CVector(1.0, 0.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

    // ����������������
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glutSolidCone(0.15, 1.0, 16, 8);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== ����״̬ ==================
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}