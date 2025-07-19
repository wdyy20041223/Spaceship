// Astronaut.cpp
#include "Astronaut.h"
#include <GL/glew.h>      // OpenGL��չ
#include "glut.h"   // GLUT���߰�
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
    astronaut.angleStep = 1.8f;   // Ĭ��ת��Ƕ�����
    astronaut.speedLen = 0.001f;    // ��ʼ�ٶ�
    astronaut.leftRightAngle = 0.0f; // ��ʼ����Ƕ�
    astronaut.position = CVector(0, -0.01, 0); // ��ʼλ�ã�Y=1ģ�����߶ȣ�
    astronaut.direction = CVector(0, 0, 1);
    astronaut.finalDir = CVector(0, 0, 1);
    astronaut.cameraDistLen = 0.3;
    ControlingGlobal = true;

    astronaut.headTexture = LoadTexture("textures/astronaut_head.jpg");
    astronaut.bodyTexture = LoadTexture("textures/astronaut_body.jpg");
    astronaut.armTexture = LoadTexture("textures/astronaut_arm.jpg");
    astronaut.legTexture = LoadTexture("textures/astronaut_leg.jpg");
    astronaut.hairTexture = LoadTexture("textures/astronaut_hair.jpg");
    astronaut.faceTexture = LoadTexture("textures/astronaut_face.jpg");
    astronaut.eyeTexture = LoadTexture("textures/eye2.png");
    astronaut.noseTexture = LoadTexture("textures/nose.png");
    astronaut.mouthTexture = LoadTexture("textures/mouth.png");

    astronaut.packageTexture = LoadTexture("textures/suit_package.png");
    astronaut.beforeTexture = LoadTexture("textures/before(1).png");
}

void calculatehead() {
    float aa = 0.03;
    // 1. ��ȡ�ɴ��ı任���󣨻�����Ԫ����
    transMat1.SetTrans(myShip.position);
    CMatrix shipRotMat = myShip.orientation.ToMatrix(); // ����Ԫ��������ת����

    // ��Ϸɴ���ƽ�ƺ���ת
    finalMat = transMat1 * shipRotMat; // ˳����ƽ�ƣ�����ת

    transMat1.SetTrans(astronaut.position);
    rotateMat1.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    scaleMat1.SetScale(CVector(0.2 * aa, 0.2 * aa, 0.2 * aa));
    finalMat = finalMat * transMat1 * rotateMat1 * scaleMat1;

    CVector headPosition = CVector(
        finalMat.m03 + 2.2 * finalMat.m01,  // X������ƽ��X + 2.2*Y��X����
        finalMat.m13 + 2.2 * finalMat.m11,  // Y������ƽ��Y + 2.2*Y��Y����
        finalMat.m23 + 2.2 * finalMat.m21   // Z������ƽ��Z + 2.2*Y��Z����
    );
    astronaut.head = headPosition;
}

void drawTexturedQuad(CVector pos, float width, float height, float rotAngle, CVector axis) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rotAngle, axis.x, axis.y, axis.z);

    CVector normal(0, 0, 1); // ��ʼ���ߣ���ֱ���ı���ƽ�棩
    CMatrix rotMat;
    rotMat.SetRotate(rotAngle, axis);
    normal = rotMat.vecMul(normal).Normalized(); // Ӧ����ͬ��ת�任

    glNormal3f(normal.x, normal.y, normal.z);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, -height / 2, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, -height / 2, 0);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width / 2, height / 2, 0);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-width / 2, height / 2, 0);
    glEnd();

    glPopMatrix();
    glDisable(GL_BLEND);
}


void drawAstronaut() {

    // ���֮ǰ�İ�Χ��
    astronaut.collisionBoxes.clear();

    float aa = 0.03;

    glPushMatrix();

    // 1. ��ȡ�ɴ��ı任���󣨻�����Ԫ����
    transMat1.SetTrans(myShip.position);
    CMatrix shipRotMat = myShip.orientation.ToMatrix(); // ����Ԫ��������ת����

    // ��Ϸɴ���ƽ�ƺ���ת
    finalMat = transMat1 * shipRotMat; // ˳����ƽ�ƣ�����ת

    transMat1.SetTrans(astronaut.position);
    rotateMat1.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    scaleMat1.SetScale(CVector(0.2 * aa, 0.2 * aa, 0.2 * aa));
    finalMat = finalMat * transMat1 * rotateMat1 * scaleMat1;

    glMultMatrixf(finalMat);

    CMatrix localRotMat;
    localRotMat.SetRotate(astronaut.allAngle.h, CVector(0, 1, 0));
    CVector localDir = localRotMat.vecMul(CVector(0, 0, 1)); // ��ʼǰ����ΪZ��
    astronaut.direction = localDir.Normalized();
    // �滻ԭ�� localRotMat ���㲿��
    localDir = CVector(finalMat.m02, finalMat.m12, finalMat.m22); // �ӱ任��������ȡZ�᷽��
    astronaut.finalDir = localDir.Normalized();


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
    // ================== ����̫���˲������� ==================
    GLfloat mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_shininess = 30.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    // ���ù��ռ���
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);  // ֻ����L2��L3��Դ
    glEnable(GL_LIGHT3);
    glEnable(GL_NORMALIZE);  // �Զ���һ������

    // ================== �������� ==================
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // ================== ͷ�������壩==================
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 2.2, 0));  // ͷ��λ��
    glMultMatrixf(transMat1);

    // ͷ������ͷ��
    glBindTexture(GL_TEXTURE_2D, astronaut.faceTexture);
    glColor3f(1, 1, 1);
    glutSolidSphere(0.3, 16, 16);

    // �沿���ף���ͻ�������棩
    glPushMatrix();
    glTranslatef(0, 0, 0.05);

    // 1. ��
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0.06, 0.24));
    rotateMat1.SetRotate(180, CVector(0, 1, 0));
    CMatrix finalMat2 = transMat1 * rotateMat1;
    rotateMat1.SetRotate(15, CVector(1, 0, 00));
    finalMat2 = finalMat2 * rotateMat1;
    glMultMatrixf(finalMat2);
    glBindTexture(GL_TEXTURE_2D, astronaut.eyeTexture);
    // ʹ�����ƫ��ȷ���۾�����ǰ��
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0, -1.0);
    drawTexturedQuad(CVector(0, 0, 0), 0.3, 0.15, 0, CVector(1, 0, 0));
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopMatrix();

    // 2. ���ӣ�ƽ����ͼ��
    glPushMatrix();
    transMat1.SetTrans(CVector(0, -0.025, 0.26));
    rotateMat1.SetRotate(180, CVector(0, 1, 0));
    scaleMat1.SetScale(CVector(1.0, 1.0, 1.0));
    finalMat2 = transMat1 * rotateMat1 * scaleMat1;
    glMultMatrixf(finalMat2);
    // ʹ���������ƽ�����
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0, -1.0);
    glBindTexture(GL_TEXTURE_2D, astronaut.noseTexture);
    drawTexturedQuad(CVector(0, 0, 0), 0.08, 0.08, 0, CVector(1, 0, 0));
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopMatrix();

    // 3. �첿
    glPushMatrix();
    transMat1.SetTrans(CVector(0, -0.14, 0.23));
    rotateMat1.SetRotate(15, CVector(1, 0, 0));
    finalMat2 = transMat1 * rotateMat1;
    glMultMatrixf(finalMat2);

    glBindTexture(GL_TEXTURE_2D, astronaut.mouthTexture);
    drawTexturedQuad(CVector(0, 0, 0), 0.14, 0.14, 0, CVector(1, 0, 0));
    glPopMatrix();

    glPopMatrix();  // �沿���׽���

    // ͷ������������Ϊ��Բ�Σ�
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0.13, 0));
    glMultMatrixf(transMat1);
    glBindTexture(GL_TEXTURE_2D, astronaut.hairTexture);

    // ���û����ʹͷ����Ե��Ȼ����ͷ��
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ����������ͷ��
    GLUquadricObj* hairQuad = gluNewQuadric();
    gluQuadricTexture(hairQuad, GL_TRUE);
    gluQuadricNormals(hairQuad, GLU_SMOOTH);  // ȷ��������ȷ

    // �ؼ����ã��ü�ƽ���Դ�������Ч��
    glEnable(GL_CLIP_PLANE0);
    GLdouble clipPlane[] = { 0.0, 1.0, 0.0, 0.0 };  // y<0�Ĳ��ֱ��ü�
    glClipPlane(GL_CLIP_PLANE0, clipPlane);

    // ���ư����壨ֻ�����ϰ벿��
    gluSphere(hairQuad, 0.3, 32, 16);  // ʹ���㹻�ķֶ���ʹ����ƽ��

    glDisable(GL_CLIP_PLANE0);  // �ָ��ü�ƽ������
    gluDeleteQuadric(hairQuad);

    glDisable(GL_BLEND);  // ���û��
    glPopMatrix();  // ͷ������

    glPopMatrix();  // ͷ������

    // ================== ���ɣ������壩==================
    glBindTexture(GL_TEXTURE_2D, astronaut.bodyTexture);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 1.4, 0));
    glMultMatrixf(transMat1);
    glColor3f(1, 1, 1);

    // ���浱ǰģ����ͼ����
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();  // ����Ϊ��λ����

    // ��ȷ�������������ɲ���
    GLfloat sPlane[] = { 1.0f, 0.0f, 0.0f, 0.5f };  // s = x + 0.5 (��Χ [0,1])
    GLfloat tPlane[] = { 0.0f, 1.0f, 0.0f, 0.5f };   // t = y + 0.5 (��Χ [0,1])

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlane);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    // �ָ�ģ����ͼ����
    glPopMatrix();

    // Ӧ������ - ������������
    scaleMat1.SetScale(CVector(0.444, 1, 0.444));  // ����X/Z����һ��
    glMultMatrixf(scaleMat1);

    // ���������Χ��
    AABB bodyBox;
    bodyBox.partName = "Body";
    bodyBox.min = CVector(-0.5, -0.5, -0.5); // ������ߴ�1.0
    bodyBox.max = CVector(0.5, 0.5, 0.5);
    bodyBox.worldTransform = finalMat * transMat1 * scaleMat1;
    astronaut.collisionBoxes.push_back(bodyBox);

    // ����������
    glutSolidCube(1.0);
    //�������
    glPushMatrix();
    transMat1.SetTrans(CVector(0,0,-0.501));
    finalMat2 = transMat1;
    glMultMatrixf(finalMat2);

    glBindTexture(GL_TEXTURE_2D, astronaut.packageTexture);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0, -1.0);
    float temp = 1.3;
    drawTexturedQuad(CVector(0, 0, 0), 0.761*temp, 0.761 *temp, 0, CVector(1, 0, 0));
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopMatrix();

    //���ǰ��
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, 0.501));
    finalMat2 = transMat1;
    glMultMatrixf(finalMat2);
    glBindTexture(GL_TEXTURE_2D, astronaut.beforeTexture);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0, -1.0);
    temp = 1.3;
    drawTexturedQuad(CVector(0, 0, 0), 0.761 * temp, 0.761 * temp, 0, CVector(1, 0, 0));
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopMatrix();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== �ұۣ�Բ׶�壩==================
    glBindTexture(GL_TEXTURE_2D, astronaut.armTexture);
    glPushMatrix();
    glColor3f(1, 1, 1);
    transMat1.SetTrans(CVector(0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1* rotateMat1);

    // �����ұ۰�Χ��
    AABB rightArmBox;
    rightArmBox.partName = "LeftArm";
    rightArmBox.min = CVector(-0.1, -0.1, 0.0); // Բ׶����뾶0.1���߶�0.8
    rightArmBox.max = CVector(0.1, 0.1, 0.8);
    rightArmBox.worldTransform = finalMat * transMat1 * rotateMat1;
    astronaut.collisionBoxes.push_back(rightArmBox);  

    // ��ӷ��߼��㣨Բ׶����Ҫ���⴦��
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);  // �Զ����ɷ���

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // �޸�������Բ׶�壨�����棩
    gluCylinder(quadric, 0.1, 0.0, 0.8, 16, 8);  // Բ׶����
    gluDisk(quadric, 0.0, 0.1, 16, 1);           // ��ӵ���Բ��

    gluDeleteQuadric(quadric);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== ��ۣ�Բ׶�壩================== 
    glPushMatrix();
    glColor3f(1, 1, 1); // ������ɫΪ��ɫ
    transMat1.SetTrans(CVector(-0.4, 1.7, 0.0));
    rotateMat1.SetRotate(0, CVector(0.0, 1.0, 0.0));
    glMultMatrixf(transMat1* rotateMat1);

    // ������۰�Χ��
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

    // �޸�������Բ׶�壨�����棩
    gluCylinder(quadric, 0.1, 0.0, 0.8, 16, 8);  // Բ׶����
    gluDisk(quadric, 0.0, 0.1, 16, 1);           // ��ӵ���Բ��

    gluDeleteQuadric(quadric);
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
    glMultMatrixf(transMat1* rotateMat1* rotateMat2);

    // �������Ȱ�Χ��
    AABB rightLegBox;
    rightLegBox.partName = "leftLeg";
    rightLegBox.min = CVector(-0.15, -0.15, 0.0); // Բ׶����뾶0.15���߶�1.0
    rightLegBox.max = CVector(0.15, 0.15, 1.0);
    rightLegBox.worldTransform = finalMat * transMat1 * rotateMat1 * rotateMat2;
    astronaut.collisionBoxes.push_back(rightLegBox);

   
    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    // ����������������
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

    // �޸�������Բ׶�壨�����棩
    gluCylinder(quadric, 0.15, 0.0, 1.0, 16, 8);  // Բ׶����
    gluDisk(quadric, 0.0, 0.15, 16, 1);           // ��ӵ���Բ��

    gluDeleteQuadric(quadric);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== ���ȣ�Բ׶�壩==================
    glPushMatrix();
    glColor3f(1, 1, 1); // ������ɫΪ��ɫ
    transMat1.SetTrans(CVector(-0.2, 0.8, 0.0));
    rotateMat1.SetRotate(90, CVector(1.0, 0.0, 0.0));
    rotateMat2.SetRotate(astronaut.leftLegAngle, CVector(1.0, 0.0, 0.0));
    glMultMatrixf(transMat1* rotateMat1* rotateMat2);

    // �������Ȱ�Χ��
    AABB leftLegBox;
    leftLegBox.partName = "RightLeg";
    leftLegBox.min = CVector(-0.15, -0.15, 0.0);
    leftLegBox.max = CVector(0.15, 0.15, 1.0);
    leftLegBox.worldTransform = finalMat * transMat1 * rotateMat1 * rotateMat2;
    astronaut.collisionBoxes.push_back(leftLegBox);

    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    // ����������������
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    // �޸�������Բ׶�壨�����棩
    gluCylinder(quadric, 0.15, 0.0, 1.0, 16, 8);  // Բ׶����
    gluDisk(quadric, 0.0, 0.15, 16, 1);           // ��ӵ���Բ��

    gluDeleteQuadric(quadric);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    // ================== ����״̬ ==================
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);  // ���ù��գ������������������Ҫ�������ã�
    glPopMatrix();

    
}


void DrawAABB(const AABB& box, char category) {
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_LINE_BIT);
    glPushMatrix();

    // Ӧ������任����
    //glMultMatrixf(box.worldTransform);

    glScalef(1.03f, 1.03f, 1.03f);

    // ���ù��պ�����
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // �����߿�ģʽ
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);

    // �������������ɫ
    if (category == 'a')
        glColor3f(1.0f, 1.0f, 0.0f);
    else if (category == 's')
        glColor3f(0.0f, 0.0f, 1.0f);

    // ���ư�Χ�� - ʹ��8��������Ƴ������6����
    glBegin(GL_QUADS);

    // ���棨+Z����
    glVertex3f(box.pos[0].x, box.pos[0].y, box.pos[0].z);
    glVertex3f(box.pos[1].x, box.pos[1].y, box.pos[1].z);
    glVertex3f(box.pos[5].x, box.pos[5].y, box.pos[5].z);
    glVertex3f(box.pos[4].x, box.pos[4].y, box.pos[4].z);

    // ���棨-Z����
    glVertex3f(box.pos[2].x, box.pos[2].y, box.pos[2].z);
    glVertex3f(box.pos[6].x, box.pos[6].y, box.pos[6].z);
    glVertex3f(box.pos[7].x, box.pos[7].y, box.pos[7].z);
    glVertex3f(box.pos[3].x, box.pos[3].y, box.pos[3].z);

    // ǰ�棨-X����
    glVertex3f(box.pos[0].x, box.pos[0].y, box.pos[0].z);
    glVertex3f(box.pos[2].x, box.pos[2].y, box.pos[2].z);
    glVertex3f(box.pos[3].x, box.pos[3].y, box.pos[3].z);
    glVertex3f(box.pos[1].x, box.pos[1].y, box.pos[1].z);

    // ���棨+X����
    glVertex3f(box.pos[4].x, box.pos[4].y, box.pos[4].z);
    glVertex3f(box.pos[5].x, box.pos[5].y, box.pos[5].z);
    glVertex3f(box.pos[7].x, box.pos[7].y, box.pos[7].z);
    glVertex3f(box.pos[6].x, box.pos[6].y, box.pos[6].z);

    // ���棨+Y����
    glVertex3f(box.pos[0].x, box.pos[0].y, box.pos[0].z);
    glVertex3f(box.pos[4].x, box.pos[4].y, box.pos[4].z);
    glVertex3f(box.pos[6].x, box.pos[6].y, box.pos[6].z);
    glVertex3f(box.pos[2].x, box.pos[2].y, box.pos[2].z);

    // ���棨-Y����
    glVertex3f(box.pos[1].x, box.pos[1].y, box.pos[1].z);
    glVertex3f(box.pos[3].x, box.pos[3].y, box.pos[3].z);
    glVertex3f(box.pos[7].x, box.pos[7].y, box.pos[7].z);
    glVertex3f(box.pos[5].x, box.pos[5].y, box.pos[5].z);

    glEnd();

    // �ָ�״̬
    glPopMatrix();
    glPopAttrib();
}

void DrawAABB2(const AABB& box, char category) {
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_LINE_BIT);
    glPushMatrix();

    // Ӧ������任����
    glMultMatrixf(box.worldTransform);
    glScalef(1.01f, 1.01f, 1.01f);

    // ���ù��պ�����
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // �����߿�ģʽ
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);

    // �������������ɫ
    if (category == 'a')
        glColor3f(1.0f, 1.0f, 0.0f);
    else if (category == 's')
        glColor3f(0.0f, 0.0f, 1.0f);
    else
        glColor3f(0.0f, 1.0f, 0.0f);

    // ���ư�Χ��
    glBegin(GL_QUADS);
    // ǰ��
    glVertex3f(box.min.x, box.min.y, box.max.z);
    glVertex3f(box.max.x, box.min.y, box.max.z);
    glVertex3f(box.max.x, box.max.y, box.max.z);
    glVertex3f(box.min.x, box.max.y, box.max.z);

    // ����
    glVertex3f(box.min.x, box.min.y, box.min.z);
    glVertex3f(box.min.x, box.max.y, box.min.z);
    glVertex3f(box.max.x, box.max.y, box.min.z);
    glVertex3f(box.max.x, box.min.y, box.min.z);

    // ����
    glVertex3f(box.min.x, box.max.y, box.min.z);
    glVertex3f(box.min.x, box.max.y, box.max.z);
    glVertex3f(box.max.x, box.max.y, box.max.z);
    glVertex3f(box.max.x, box.max.y, box.min.z);

    // ����
    glVertex3f(box.min.x, box.min.y, box.min.z);
    glVertex3f(box.max.x, box.min.y, box.min.z);
    glVertex3f(box.max.x, box.min.y, box.max.z);
    glVertex3f(box.min.x, box.min.y, box.max.z);

    // ����
    glVertex3f(box.min.x, box.min.y, box.min.z);
    glVertex3f(box.min.x, box.min.y, box.max.z);
    glVertex3f(box.min.x, box.max.y, box.max.z);
    glVertex3f(box.min.x, box.max.y, box.min.z);

    // ����
    glVertex3f(box.max.x, box.min.y, box.max.z);
    glVertex3f(box.max.x, box.min.y, box.min.z);
    glVertex3f(box.max.x, box.max.y, box.min.z);
    glVertex3f(box.max.x, box.max.y, box.max.z);
    glEnd();

    // �ָ�״̬
    glPopMatrix();
    glPopAttrib();
}