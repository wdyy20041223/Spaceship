// Astronaut.cpp
#include "Astronaut.h"
#include <GL/glew.h>      // OpenGL扩展
#include "glut.h"   // GLUT工具包
#include "CMatrix.h"
#include "ship.h"
#include "camera.h"
#include "planet.h"

extern CMatrix transMat1, transMat2, rotateMat1, rotateMat2, scaleMat1, scaleMat2, finalMat;
extern Astronaut astronaut;
extern ship myShip;
extern Camera astronautCamera;
extern bool ControlingGlobal;
char a = 'a';

void initAstronaut() {
    astronaut.angleStep = 1.8f;   // 默认转向角度增量
    astronaut.speedLen = 0.001f;    // 初始速度
    astronaut.leftRightAngle = 0.0f; // 初始朝向角度
    astronaut.position = CVector(0, -0.01, 0); // 初始位置（Y=1模拟地面高度）
    astronaut.direction = CVector(0, 0, 1);
    astronaut.finalDir = CVector(0, 0, 1);
    astronaut.cameraDistLen = 0.3;
    ControlingGlobal = true;

    astronaut.headTexture = LoadTexture("textures/astronaut_head.jpg");
    astronaut.bodyTexture = LoadTexture("textures/astronaut_body.jpg");
    astronaut.armTexture = LoadTexture("textures/astronaut_arm.jpg");
    astronaut.legTexture = LoadTexture("textures/astronaut_leg.jpg");
}

void calculatehead() {
    float aa = 0.03;


    // 1. 获取飞船的变换矩阵（基于四元数）
    transMat1.SetTrans(myShip.position);
    CMatrix shipRotMat = myShip.orientation.ToMatrix(); // 从四元数生成旋转矩阵

    // 组合飞船的平移和旋转
    finalMat = transMat1 * shipRotMat; // 顺序：先平移，再旋转

    transMat1.SetTrans(astronaut.position);
    rotateMat1.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    scaleMat1.SetScale(CVector(0.2 * aa, 0.2 * aa, 0.2 * aa));
    finalMat = finalMat * transMat1 * rotateMat1 * scaleMat1;

    CVector headPosition = CVector(
        finalMat.m03 + 2.2 * finalMat.m01,  // X分量：平移X + 2.2*Y轴X方向
        finalMat.m13 + 2.2 * finalMat.m11,  // Y分量：平移Y + 2.2*Y轴Y方向
        finalMat.m23 + 2.2 * finalMat.m21   // Z分量：平移Z + 2.2*Y轴Z方向
    );
    astronaut.head = headPosition;
}

void drawAstronaut() {
    // 清空之前的包围盒
    astronaut.collisionBoxes.clear();


    float aa = 0.03;

    glPushMatrix();

    // 1. 获取飞船的变换矩阵（基于四元数）
    transMat1.SetTrans(myShip.position);
    CMatrix shipRotMat = myShip.orientation.ToMatrix(); // 从四元数生成旋转矩阵

    // 组合飞船的平移和旋转
    finalMat = transMat1 * shipRotMat; // 顺序：先平移，再旋转

    transMat1.SetTrans(astronaut.position);
    rotateMat1.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    scaleMat1.SetScale(CVector(0.2 * aa, 0.2 * aa, 0.2 * aa));
    finalMat = finalMat * transMat1 * rotateMat1 * scaleMat1;

    glMultMatrixf(finalMat);

    CMatrix localRotMat;
    localRotMat.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    CVector localDir = localRotMat.vecMul(CVector(0, 0, 1)); // 初始前方向为Z轴
    astronaut.direction = localDir.Normalized();
    // 替换原有 localRotMat 计算部分
    localDir = CVector(finalMat.m02, finalMat.m12, finalMat.m22); // 从变换矩阵中提取Z轴方向
    astronaut.finalDir = localDir.Normalized();


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
    // ================== 设置太空人材质属性 ==================
    GLfloat mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_shininess = 30.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    // 启用光照计算
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);  // 只考虑L2和L3光源
    glEnable(GL_LIGHT3);
    glEnable(GL_NORMALIZE);  // 自动归一化法线

    // ================== 启用纹理 ==================
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // ================== 头部（球体）==================
    glBindTexture(GL_TEXTURE_2D, astronaut.headTexture);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 2.2, 0));
    glMultMatrixf(transMat1);
    glColor3f(1, 1, 1);

    // 计算头部包围盒
    AABB headBox;
    headBox.partName = "Head";
    headBox.min = CVector(-0.3, -0.3, -0.3); // 球体半径0.3
    headBox.max = CVector(0.3, 0.3, 0.3);
    headBox.worldTransform = finalMat * transMat1;
    astronaut.collisionBoxes.push_back(headBox);

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

    // 计算身体包围盒
    AABB bodyBox;
    bodyBox.partName = "Body";
    bodyBox.min = CVector(-0.5, -0.5, -0.5); // 立方体尺寸1.0
    bodyBox.max = CVector(0.5, 0.5, 0.5);
    bodyBox.worldTransform = finalMat * transMat1 * scaleMat1;
    astronaut.collisionBoxes.push_back(bodyBox);

   

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
    glColor3f(1, 1, 1);
    transMat1.SetTrans(CVector(0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1* rotateMat1);

    // 计算右臂包围盒
    AABB rightArmBox;
    rightArmBox.partName = "LeftArm";
    rightArmBox.min = CVector(-0.1, -0.1, 0.0); // 圆锥底面半径0.1，高度0.8
    rightArmBox.max = CVector(0.1, 0.1, 0.8);
    rightArmBox.worldTransform = finalMat * transMat1 * rotateMat1;
    astronaut.collisionBoxes.push_back(rightArmBox);  

    // 添加法线计算（圆锥体需要特殊处理）
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);  // 自动生成法线

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // 修复：绘制圆锥体（带底面）
    gluCylinder(quadric, 0.1, 0.0, 0.8, 16, 8);  // 圆锥侧面
    gluDisk(quadric, 0.0, 0.1, 16, 1);           // 添加底面圆盘

    gluDeleteQuadric(quadric);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== 左臂（圆锥体）================== 
    glPushMatrix();
    glColor3f(1, 1, 1); // 设置颜色为白色
    transMat1.SetTrans(CVector(-0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1* rotateMat1);

    // 计算左臂包围盒
    AABB leftArmBox;
    leftArmBox.partName = "RightArm";
    leftArmBox.min = CVector(-0.1, -0.1, 0.0);
    leftArmBox.max = CVector(0.1, 0.1, 0.8);
    leftArmBox.worldTransform = finalMat * transMat1 * rotateMat1;
    astronaut.collisionBoxes.push_back(leftArmBox);

   

    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    // 修复：绘制圆锥体（带底面）
    gluCylinder(quadric, 0.1, 0.0, 0.8, 16, 8);  // 圆锥侧面
    gluDisk(quadric, 0.0, 0.1, 16, 1);           // 添加底面圆盘

    gluDeleteQuadric(quadric);
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
    glMultMatrixf(transMat1* rotateMat1* rotateMat2);

    // 计算右腿包围盒
    AABB rightLegBox;
    rightLegBox.partName = "leftLeg";
    rightLegBox.min = CVector(-0.15, -0.15, 0.0); // 圆锥底面半径0.15，高度1.0
    rightLegBox.max = CVector(0.15, 0.15, 1.0);
    rightLegBox.worldTransform = finalMat * transMat1 * rotateMat1 * rotateMat2;
    astronaut.collisionBoxes.push_back(rightLegBox);

    

    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    // 启用纹理坐标生成
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

    // 修复：绘制圆锥体（带底面）
    gluCylinder(quadric, 0.15, 0.0, 1.0, 16, 8);  // 圆锥侧面
    gluDisk(quadric, 0.0, 0.15, 16, 1);           // 添加底面圆盘

    gluDeleteQuadric(quadric);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== 左腿（圆锥体）==================
    glPushMatrix();
    glColor3f(1, 1, 1); // 设置颜色为白色
    transMat1.SetTrans(CVector(-0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0));
    rotateMat2.SetRotate(astronaut.leftLegAngle, CVector(1.0, 0.0, 0.0));
    glMultMatrixf(transMat1* rotateMat1* rotateMat2);

    // 计算左腿包围盒
    AABB leftLegBox;
    leftLegBox.partName = "RightLeg";
    leftLegBox.min = CVector(-0.15, -0.15, 0.0);
    leftLegBox.max = CVector(0.15, 0.15, 1.0);
    leftLegBox.worldTransform = finalMat * transMat1 * rotateMat1 * rotateMat2;
    astronaut.collisionBoxes.push_back(leftLegBox);

    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    // 启用纹理坐标生成
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    // 修复：绘制圆锥体（带底面）
    gluCylinder(quadric, 0.15, 0.0, 1.0, 16, 8);  // 圆锥侧面
    gluDisk(quadric, 0.0, 0.15, 16, 1);           // 添加底面圆盘

    gluDeleteQuadric(quadric);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== 清理状态 ==================
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);  // 禁用光照（假设其他物体可能需要单独设置）
    glPopMatrix();

    
}


void DrawAABB(const AABB& box, char category) {
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_LINE_BIT);
    glPushMatrix();

    // 应用世界变换矩阵
    //glMultMatrixf(box.worldTransform);

    glScalef(1.03f, 1.03f, 1.03f);

    // 禁用光照和纹理
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // 设置线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);

    // 根据类别设置颜色
    if (category == 'a')
        glColor3f(1.0f, 1.0f, 0.0f);
    else if (category == 's')
        glColor3f(0.0f, 0.0f, 1.0f);

    // 绘制包围盒 - 使用8个顶点绘制长方体的6个面
    glBegin(GL_QUADS);

    // 底面（+Z方向）
    glVertex3f(box.pos[0].x, box.pos[0].y, box.pos[0].z);
    glVertex3f(box.pos[1].x, box.pos[1].y, box.pos[1].z);
    glVertex3f(box.pos[5].x, box.pos[5].y, box.pos[5].z);
    glVertex3f(box.pos[4].x, box.pos[4].y, box.pos[4].z);

    // 顶面（-Z方向）
    glVertex3f(box.pos[2].x, box.pos[2].y, box.pos[2].z);
    glVertex3f(box.pos[6].x, box.pos[6].y, box.pos[6].z);
    glVertex3f(box.pos[7].x, box.pos[7].y, box.pos[7].z);
    glVertex3f(box.pos[3].x, box.pos[3].y, box.pos[3].z);

    // 前面（-X方向）
    glVertex3f(box.pos[0].x, box.pos[0].y, box.pos[0].z);
    glVertex3f(box.pos[2].x, box.pos[2].y, box.pos[2].z);
    glVertex3f(box.pos[3].x, box.pos[3].y, box.pos[3].z);
    glVertex3f(box.pos[1].x, box.pos[1].y, box.pos[1].z);

    // 后面（+X方向）
    glVertex3f(box.pos[4].x, box.pos[4].y, box.pos[4].z);
    glVertex3f(box.pos[5].x, box.pos[5].y, box.pos[5].z);
    glVertex3f(box.pos[7].x, box.pos[7].y, box.pos[7].z);
    glVertex3f(box.pos[6].x, box.pos[6].y, box.pos[6].z);

    // 左面（+Y方向）
    glVertex3f(box.pos[0].x, box.pos[0].y, box.pos[0].z);
    glVertex3f(box.pos[4].x, box.pos[4].y, box.pos[4].z);
    glVertex3f(box.pos[6].x, box.pos[6].y, box.pos[6].z);
    glVertex3f(box.pos[2].x, box.pos[2].y, box.pos[2].z);

    // 右面（-Y方向）
    glVertex3f(box.pos[1].x, box.pos[1].y, box.pos[1].z);
    glVertex3f(box.pos[3].x, box.pos[3].y, box.pos[3].z);
    glVertex3f(box.pos[7].x, box.pos[7].y, box.pos[7].z);
    glVertex3f(box.pos[5].x, box.pos[5].y, box.pos[5].z);

    glEnd();

    // 恢复状态
    glPopMatrix();
    glPopAttrib();
}

void DrawAABB2(const AABB& box, char category) {
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_LINE_BIT);
    glPushMatrix();

    // 应用世界变换矩阵
    glMultMatrixf(box.worldTransform);
    glScalef(1.01f, 1.01f, 1.01f);

    // 禁用光照和纹理
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // 设置线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);

    // 根据类别设置颜色
    if (category == 'a')
        glColor3f(1.0f, 1.0f, 0.0f);
    else if (category == 's')
        glColor3f(0.0f, 0.0f, 1.0f);
    else
        glColor3f(0.0f, 1.0f, 0.0f);

    // 绘制包围盒
    glBegin(GL_QUADS);
    // 前面
    glVertex3f(box.min.x, box.min.y, box.max.z);
    glVertex3f(box.max.x, box.min.y, box.max.z);
    glVertex3f(box.max.x, box.max.y, box.max.z);
    glVertex3f(box.min.x, box.max.y, box.max.z);

    // 后面
    glVertex3f(box.min.x, box.min.y, box.min.z);
    glVertex3f(box.min.x, box.max.y, box.min.z);
    glVertex3f(box.max.x, box.max.y, box.min.z);
    glVertex3f(box.max.x, box.min.y, box.min.z);

    // 上面
    glVertex3f(box.min.x, box.max.y, box.min.z);
    glVertex3f(box.min.x, box.max.y, box.max.z);
    glVertex3f(box.max.x, box.max.y, box.max.z);
    glVertex3f(box.max.x, box.max.y, box.min.z);

    // 下面
    glVertex3f(box.min.x, box.min.y, box.min.z);
    glVertex3f(box.max.x, box.min.y, box.min.z);
    glVertex3f(box.max.x, box.min.y, box.max.z);
    glVertex3f(box.min.x, box.min.y, box.max.z);

    // 左面
    glVertex3f(box.min.x, box.min.y, box.min.z);
    glVertex3f(box.min.x, box.min.y, box.max.z);
    glVertex3f(box.min.x, box.max.y, box.max.z);
    glVertex3f(box.min.x, box.max.y, box.min.z);

    // 右面
    glVertex3f(box.max.x, box.min.y, box.max.z);
    glVertex3f(box.max.x, box.min.y, box.min.z);
    glVertex3f(box.max.x, box.max.y, box.min.z);
    glVertex3f(box.max.x, box.max.y, box.max.z);
    glEnd();

    // 恢复状态
    glPopMatrix();
    glPopAttrib();
}