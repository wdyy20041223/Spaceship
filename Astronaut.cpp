// Astronaut.cpp
#include "Astronaut.h"
#include <GL/glew.h>      // OpenGL扩展
#include <GL/glut.h>      // GLUT工具包
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
    astronaut.angleStep = 1.8f;   // 默认转向角度增量
    astronaut.speedLen = 0.001f;    // 初始速度
    astronaut.leftRightAngle = 0.0f; // 初始朝向角度
    astronaut.position = CVector(0, -0.01, 0); // 初始位置（Y=1模拟地面高度）
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

    // 飞船基础变换（位置 + 旋转）
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
    // ================== 启用纹理 ==================
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // ================== 头部（球体）==================
    glBindTexture(GL_TEXTURE_2D, astronaut.headTexture);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 2.2, 0));
    glMultMatrixf(transMat1);
    glColor3f(1, 1, 1);

    // 启用球面映射纹理坐标
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glutSolidSphere(0.3, 32, 32);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== 躯干（立方体）==================
    glBindTexture(GL_TEXTURE_2D, astronaut.bodyTexture);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 1.4, 0));
    scaleMat1.SetScale(CVector(0.444, 1, 0.333));
    glMultMatrixf(transMat1 * scaleMat1);

    // 调整后的纹理生成平面参数
    GLfloat sPlane[] = { 1.0f, 0.0f, 0.0f, 0.0f };  // X轴方向，系数从2.25降为1.0
    GLfloat tPlane[] = { 0.0f, 1.0f, 0.0f, 0.0f };   // Y轴方向

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

    // ================== 右臂（圆锥体）==================
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

    // ================== 左臂（圆锥体）================== 
    glPushMatrix();
    glColor3f(1, 1, 1); // 设置颜色为白色
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

    // ================== 右腿（圆锥体）==================
    glBindTexture(GL_TEXTURE_2D, astronaut.legTexture);
    glPushMatrix();
    glColor3f(1, 1, 1); // 设置颜色为白色
    transMat1.SetTrans(CVector(0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0));
    rotateMat2.SetRotate(astronaut.rightLegAngle, CVector(1.0, 0.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

    // 启用纹理坐标生成
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glutSolidCone(0.15, 1.0, 16, 8);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== 左腿（圆锥体）==================
    glPushMatrix();
    glColor3f(1, 1, 1); // 设置颜色为白色
    transMat1.SetTrans(CVector(-0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0));
    rotateMat2.SetRotate(astronaut.leftLegAngle, CVector(1.0, 0.0, 0.0));
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

    // 启用纹理坐标生成
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glutSolidCone(0.15, 1.0, 16, 8);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== 清理状态 ==================
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}