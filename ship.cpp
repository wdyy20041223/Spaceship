
#include "base.h"
#include "CMatrix.h"
#include "planet.h"


extern Camera globalCamera, astronautCamera, shipCamera;
extern bool ControllingShip;

void initShip() {
    ControllingShip = true;
    myShip.angleStep = 1.8;
    myShip.position = CVector(1, 1, 1);
    float a = 0.2,test = 1;
    myShip.speedLen = 0.01 * a;
    myShip.speedStep = 0.002 * a * test;
    myShip.autoPilot = false;

    myShip.seatTexture = LoadTexture("textures/seat.jpg");
}

void shipMove() {
    myShip.direction = getShipDir(myShip);
    myShip.speed = myShip.direction * myShip.speedLen;
    myShip.position = myShip.position + myShip.speed;
    astronautCamera.origonPos = astronautCamera.origonPos + myShip.speed;
    shipCamera.origonPos = shipCamera.origonPos + myShip.speed;
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

            shipCamera.orientation = deltaQ * astronautCamera.orientation;
            shipCamera.orientation.Normalize();
            shipCamera.UpdateEulerFromOrientation(); // ͬ��ŷ����


            myShip.leftRightAngle = yaw;
            myShip.upDownAngle = pitch;

            if (myShip.speedLen <= myShip.speedStep * 2 && myShip.speedTempLen != 0) {
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

void drawSeat(float len, float pos1, float pos2) {
    glPushMatrix();
    {
        // ����->��ת->ƽ�ƣ�ע��˳��ת��
        transMat1.SetTrans(CVector(pos1, -0.185f, pos2));
        rotateMat1.SetRotate(180.0f, CVector(0, 1, 0)); // ������ת��
        scaleMat1.SetScale(CVector(len, 0.5f, 0.51f));
        glMultMatrixf(scaleMat1 * rotateMat1 * transMat1); // ����˳��ת

        // ���棨�ֲ����ţ�
        scaleMat1.SetScale(CVector(0.5f, 0.1f, 0.2f));
        glPushMatrix();
        glMultMatrixf(scaleMat1);
        glColor3f(0.3f, 0.3f, 0.3f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // ������ƽ��->��ת->���ţ�
        transMat1.SetTrans(CVector(0.0f, 0.1f, 0.1f));
        rotateMat1.SetRotate(10.0f, CVector(1, 0, 0));
        scaleMat1.SetScale(CVector(0.5f, 0.3f, 0.05f));
        glPushMatrix();
        glMultMatrixf(transMat1 * rotateMat1 * scaleMat1);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    glPopMatrix();
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
    transMat1.SetTrans(CVector(0, 0.35, 0));
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
    // ============= ��������ڲ�װ������� =============
    glPushMatrix();
    {
        // ȫ�����ţ�ע�����˳��ӦΪ����*ԭʼ�任��
        scaleMat1.SetScale(CVector(0.5f, 0.5f, 0.5f));
        glMultMatrixf(scaleMat1);

        //----- ����̨�Ǳ��� -----
        glPushMatrix();
        {
            // ƽ��->��ת��ϣ�ע��OpenGL�����ҳ˹���
            transMat1.SetTrans(CVector(0.0f, -0.105f, -0.4f));
            rotateMat1.SetRotate(180.0f, CVector(0, 1, 0)); // ������ת��Ϊ(0,1,0)
            rotateMat2.SetRotate(5.0f, CVector(1, 0, 0));
            glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

            // �Ǳ��̻���������Ӧ���ھֲ�����ϵ��
            scaleMat1.SetScale(CVector(0.8f, 0.05f, 0.3f));
            glPushMatrix();
            glMultMatrixf(scaleMat1);
            glColor3f(0.2f, 0.2f, 0.2f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // Һ����Ļ����ƽ�ƺ����ţ�
            transMat1.SetTrans(CVector(0.0f, 0.03f, 0.1f));
            scaleMat1.SetScale(CVector(0.7f, 0.03f, 0.2f));
            glPushMatrix();
            glMultMatrixf(transMat1 * scaleMat1); // �ȼ�����������ƽ�Ƶ�����
            glColor3f(0.1f, 0.1f, 0.1f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // ������ť������ƽ�ƣ�
            glColor3f(0.0f, 0.6f, 0.0f);
            glPushMatrix();
            transMat1.SetTrans(CVector(0.3f, 0.03f, -0.1f));
            glMultMatrixf(transMat1);
            glutSolidSphere(0.03f, 8, 8);
            transMat1.SetTrans(CVector(-0.6f, 0.0f, 0.0f));
            glMultMatrixf(transMat1);
            glutSolidSphere(0.03f, 8, 8);
            glPopMatrix();
        }
        glPopMatrix();

        drawSeat(0.5, 0, 1.5);//pilot len,pos1,pos2
        drawSeat(1.91, 0, 4);//behind
        drawSeat(0.5f, -0.7, 2);//l1
        drawSeat(0.5f, -0.7, 2.5);//l2
        drawSeat(0.5f, -0.7, 3);//l3
        drawSeat(0.5f, -0.7, 3.5);//l4
        drawSeat(0.5f, 0.7, 2);//r1
        drawSeat(0.5f, 0.7, 2.5);//r2
        drawSeat(0.5f, 0.7, 3);//rl3
        drawSeat(0.5f, 0.7, 3.5);//r4
        
    }
    glPopMatrix();

    // �޸Ŀ�ʼ��ͷ��Բ׶��͸������
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT); // ����״̬
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6, 0.4, 0.7, 0.5); // ��4������0.5��ʾ50%͸����
    glutSolidCone(0.25f, 0.6f, 16, 5);
    glDisable(GL_BLEND);
    glPopAttrib(); // �ָ�ԭʼ״̬
    // �޸Ľ���

    glPopMatrix();
    gluDeleteQuadric(quadric);
    glPopMatrix();
    
    //�װ�
    glColor3f(1.0, 0.5, 0.0);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, -0.055, 0));
    scaleMat1.SetScale(CVector(0.475, 0.0002, 1.258));
    glMultMatrixf(transMat1 * scaleMat1);
    glutSolidCube(1.0);
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

