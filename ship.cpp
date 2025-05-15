
#include "base.h"
#include "CMatrix.h"


extern Camera globalCamera, astronautCamera;
extern bool ControllingShip;

void initShip() {
    ControllingShip = true;
    myShip.angleStep = 1.8;
    myShip.position = CVector(1, 1, 1);
    float a = 0.2,test = 1;
    myShip.speedLen = 0.01 * a;
    myShip.speedStep = 0.002 * a * test;
    myShip.autoPilot = false;
}


void drawShip() {
    glPushMatrix();

    transMat1.SetTrans(myShip.position);//�����Լ��ľ����� ƽ��
    rotateMat1.SetRotate(myShip.leftRightAngle, CVector(0, 1, 0));
    rotateMat2.SetRotate(-myShip.upDownAngle, CVector(1, 0, 0));
    scaleMat1.SetScale(CVector(0.2f, 0.2f, 0.2f));
    CMatrix rotateMat3;
    rotateMat3.SetRotate(myShip.rollAngle, CVector(0, 0, 1));
    finalMat = transMat1 * rotateMat1 * rotateMat2 * rotateMat3 * scaleMat1;
    glMultMatrixf(finalMat);

    glColor3f(0.0, 0.0, 1.0);//������ǣ���ʻ�գ�
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0.4, 0));
    glMultMatrixf(transMat1);
    glutSolidSphere(0.2, 4, 4);
    glPopMatrix();

    glColor3f(0.8, 0.8, 0.8);// ���壨���ṹ��
    glPushMatrix();
    GLUquadricObj* quadric = gluNewQuadric();
    transMat1.SetTrans(CVector(0, 0, -0.65f));
    glMultMatrixf(transMat1);
    gluCylinder(quadric,
        0.25f,   // ����뾶��ֱ��0.5��
        0.25f,   // ����뾶������Բ����
        1.3f,    // �߶�
        16,      // ��Ƭ����Բ���ȣ�
        4        // �ѵ���
    );
    glColor3f(1, 0, 0);
    gluDisk(quadric, 0.0f, 0.25f,18, 1);  // ����
    glPushMatrix();

    transMat1.SetTrans(CVector(0, 0, 1.3f));
    glMultMatrixf(transMat1);
    glPushMatrix();
    glColor3f(0.6, 0.4, 0.7);
    glutSolidCone(0.25f, 0.6f, 16, 5);//ͷ
    glPopMatrix();
    gluDeleteQuadric(quadric);
    glPopMatrix();
    glPopMatrix();

    glColor3f(1.0, 0.5, 0.0);// �������
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, -0.8f));
    glMultMatrixf(transMat1);
    glutSolidCone(0.2, 0.4, 8, 4);
    glPopMatrix();

    glColor3f(0.5, 0.5, 0.5);// ����
    glPushMatrix();

    transMat1.SetTrans(CVector(0.638, 0, 0));
    scaleMat1.SetScale(CVector(0.8, 0.1, 0.5));
    glMultMatrixf(transMat1 * scaleMat1);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    transMat1.SetTrans(CVector(-0.638, 0, 0));
    scaleMat1.SetScale(CVector(0.8, 0.1, 0.5));
    glMultMatrixf(transMat1 * scaleMat1);
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();

}

void shipMove() {
    myShip.direction = getShipDir(myShip);
    myShip.speed = myShip.direction * myShip.speedLen;
    myShip.position = myShip.position + myShip.speed;
}

CVector getShipDir(ship myShip) {

    float yaw = myShip.leftRightAngle * PI / 180.0f;// ���Ƕ�ת��Ϊ����
    float pitch = myShip.upDownAngle * PI / 180.0f;

    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);

    float x = sinYaw * cosPitch;
    float y = sinPitch;
    float z = cosYaw * cosPitch;

    CVector temp = CVector(x, y, z).Normalized();
    return temp; // ���ع�һ����ķ�������

}

void autoShip() {
    if (myShip.autoPilot && myShip.targetBall != nullptr) { 
        ball targrt = *(myShip.targetBall);
        float dist = myShip.position.distanceTo(targrt.centerPlace);


        if (dist > targrt.r * 1.5) {

            CVector finalDir = (targrt.centerPlace - myShip.position).Normalized();
            float angle = myShip.position.angleWith(finalDir);
            float yaw = atan2(finalDir.x, finalDir.z) * 180.0f / PI;
            float pitch = asin(finalDir.y) * 180.0f / PI;

            CVector tempdir1 = myShip.direction.Normalized();
            CQuaternion q1 = tempdir1.ToEuler().ToQuaternion(); // ��ǰ�������Ԫ��
            CQuaternion q2 = finalDir.ToEuler().ToQuaternion(); // Ŀ�귽�����Ԫ��

            // ����ӵ�ǰ��Ŀ�����ת��Ԫ��
            CQuaternion deltaQ = q2 * q1.Inverse();

            // Ӧ����ת�����
            astronautCamera.orientation = deltaQ * astronautCamera.orientation;
            astronautCamera.orientation.Normalize();
            astronautCamera.UpdateEulerFromOrientation(); // ͬ��ŷ����
            

            myShip.leftRightAngle = yaw;
            myShip.upDownAngle = pitch;

            if (myShip.speedLen <= myShip.speedStep * 2 && myShip.speedTempLen != 0 ) {
                myShip.speedLen = myShip.speedTempLen;
                myShip.speedTempLen = 0;
            }
        }
        else {
            if (myShip.speedLen >= myShip.speedStep * 2)
                myShip.speedTempLen = myShip.speedLen;
                myShip.speedLen = 0.0f;
        }

    }
}


void drawAxis() {
    CVector tempColor;
    glLineWidth(2.0f); // �����߶ο��Ϊ2.0

    tempColor.Set(1.0f, 0.0f, 0.0f);// ������ɫΪ��ɫ x
    glColor3fv(tempColor);
    glBegin(GL_LINES); // ��ʼ�����߶�
    glVertex3f(0.0f, 0.0f, 0.0f); // X�����
    glVertex3f(4.5f, 0.0f, 0.0f); // X���յ�
    glEnd();

    glPushMatrix();//��Բ׶
    transMat1.SetTrans(CVector(4.4, 0, 0));
    rotateMat1.SetRotate(90, CVector(0, 1, 0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();

    tempColor.Set(0.0f, 1.0f, 0.0f);// ������ɫΪ��ɫ y 
    glColor3fv(tempColor);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f); // Y�����
    glVertex3f(0.0f, 3.0f, 0.0f); // Y���յ�
    glEnd();

    glPushMatrix();//��Բ׶
    transMat1.SetTrans(CVector(0, 2.9, 0));
    rotateMat1.SetRotate(-90, CVector(1, 0, 0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();

    tempColor.Set(0.0f, 0.0f, 1.0f);// ������ɫΪ��ɫ z
    glColor3fv(tempColor);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f); // Z�����
    glVertex3f(0.0f, 0.0f, 4.5f); // Z���յ�
    glEnd();

    glPushMatrix();//��Բ׶
    transMat1.SetTrans(CVector(0, 0, 4.4));
    glMultMatrixf(transMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();
}