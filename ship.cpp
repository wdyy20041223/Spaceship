
#include "base.h"
#include "CMatrix.h"
#include "planet.h"


extern Camera globalCamera, astronautCamera, shipCamera;
extern bool ControllingShip;

void shipMove() {
    myShip.direction = getShipDir(myShip);
    myShip.speed = myShip.direction * myShip.speedLen;
    myShip.position = myShip.position + myShip.speed;
    astronautCamera.origonPos = astronautCamera.origonPos + myShip.speed;
    shipCamera.origonPos = shipCamera.origonPos + myShip.speed;
}
CVector getForwardDirection(const CQuaternion& orientation) {
    // ��׼ǰ�������������ʼ����ΪZ�Ḻ����
    CVector baseForward(0, 0, -1);
    return orientation * baseForward;  // ��Ԫ������������
}
CVector getUpDirection(const CQuaternion& orientation) {
    // ��׼�Ϸ�������Y��������
    CVector baseUp(0, 1, 0);
    return orientation * baseUp;
}
// ��ȡ�Ҳ෽��
CVector getRightDirection(const CQuaternion& orientation) {
    CVector baseRight(1, 0, 0);
    return orientation * baseRight;
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

            myShip.orientation = deltaQ * myShip.orientation;

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
        // ������λ�Ĳ�������
        GLfloat mat_ambient[] = { 0.6f, 0.6f, 0.f, 1.0f };   // ������
        GLfloat mat_diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };   // ������
        GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };  // ���淴��
        GLfloat mat_shininess = 80.0f;                        // �����

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

        // Ӧ�ñ任��ƽ�� + ��ת + ����
        transMat1.SetTrans(CVector(pos1, -0.185f, pos2));
        rotateMat1.SetRotate(180.0f, CVector(0, 1, 0)); // ��Y����ת180��
        scaleMat1.SetScale(CVector(len, 0.5f, 0.51f));
        glMultMatrixf(scaleMat1 * rotateMat1 * transMat1);

        // ============= ���沿�� =============
        scaleMat1.SetScale(CVector(0.5f, 0.1f, 0.2f));
        glPushMatrix();
        glMultMatrixf(scaleMat1);

        // ������λ����
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.seatTexture);
        glColor3f(1.0f, 1.0f, 1.0f); // ���ð�ɫ����ɫ

        // ���������岢ָ�����ߺ���������
        glBegin(GL_QUADS);

        // ǰ�棨���߳�ǰ��
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        // ���棨���߳���
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        // ���棨���߳��ϣ�
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        // ���棨���߳��£�
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        // ���棨���߳���
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

        // ���棨���߳��ң�
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // ============= �������� =============
        transMat1.SetTrans(CVector(0.0f, 0.1f, 0.1f));
        rotateMat1.SetRotate(10.0f, CVector(1, 0, 0)); // ��΢��б
        scaleMat1.SetScale(CVector(0.5f, 0.3f, 0.05f));
        glPushMatrix();
        glMultMatrixf(transMat1 * rotateMat1 * scaleMat1);

        // ������λ����
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.seatTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        // ��ǿ��������Ч��
        GLfloat back_mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
        GLfloat back_mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, back_mat_ambient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, back_mat_specular);

        // ���ƿ���������
        glBegin(GL_QUADS);

        // ǰ�棨���߳�ǰ��
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        // ���棨���߳���
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        // ���棨���߳��ϣ�
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        // ���棨���߳��£�
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        // ���棨���߳���
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

        // ���棨���߳��ң�
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
    glPopMatrix();
}

void initShip() {
    ControllingShip = true;
    myShip.angleStep = 1.8;
    myShip.position = CVector(2, 2, 2);
    float a = 0.2, test = 1;
    myShip.speedLen = 0.01 * a;
    myShip.speedStep = 0.002 * a * test;
    myShip.autoPilot = false;


    myShip.seatTexture = LoadTexture("textures/seat.jpg");       // ����Ƥ������
    myShip.bodyTexture = LoadTexture("textures/ship_body.jpg");  // �����������
    myShip.engineTexture = LoadTexture("textures/ship_body.jpg");  // ������������
    myShip.wingTexture = LoadTexture("textures/ship_body.jpg"); // ����װ��
    myShip.glassTexture = LoadTexture("textures/ship_body.jpg"); // ����Ч��
    myShip.panelTexture = LoadTexture("textures/ship_body.jpg"); // �������
    myShip.envMapTexture = LoadTexture("textures/ship_body.jpg"); // ����������ͼ
    myShip.floorTexture = LoadTexture("textures/ship_body.jpg");
}



void drawShip() {
    glEnable(GL_LIGHTING); // ȷ�����ռ��㿪��
    glEnable(GL_NORMALIZE); // �Զ����߹�һ��
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);

    //glDisable(GL_LIGHT0);
    //glDisable(GL_LIGHT2);
    //glDisable(GL_LIGHT3);

    glPushMatrix();
    // ʹ����Ԫ��������ת����
    CMatrix rotateMat = myShip.orientation.ToMatrix();

    // ��ϱ任����
    transMat1.SetTrans(myShip.position);
    scaleMat1.SetScale(CVector(0.2f, 0.2f, 0.2f));
    finalMat = transMat1 * rotateMat * scaleMat1;
    glMultMatrixf(finalMat);

    glColor3f(0.0, 0.0, 1.0);//������ǣ���ʻ�գ�
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0.35, 0));
    glMultMatrixf(transMat1);
    glutSolidSphere(0.2, 4, 4);
    glPopMatrix();

    glPushMatrix();
    {
        // ��������ɫ��������
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };   // �����⣨ǳ�ң�
        GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // �����䣨���ף�
        GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // ���淴�䣨���ף�
        GLfloat mat_shininess = 128.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


        GLUquadricObj* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluQuadricNormals(quadric, GLU_SMOOTH);

        transMat1.SetTrans(CVector(0, 0, -0.65f));
        glMultMatrixf(transMat1);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.bodyTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // �Ƴ���ɫ�����ӣ���Ϊ��ɫ
        glColor3f(1.0f, 1.0f, 1.0f);  // ����
        gluCylinder(quadric, 0.25f, 0.25f, 1.3f, 64, 64);

        // �޸ĵ���Բ�̲���
        GLfloat disk_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, disk_specular);
        glColor3f(0.8f, 0.8f, 0.8f);  // ǳ��ɫ
        gluDisk(quadric, 0.0f, 0.25f, 48, 1);

        gluDeleteQuadric(quadric);
        glDisable(GL_TEXTURE_2D);
    }



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
        glDisable(GL_LIGHT0);
        glPushMatrix();
        {   
            GLfloat metal_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
            GLfloat metal_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
            GLfloat metal_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
            GLfloat metal_shininess = 128.0f;

            glMaterialfv(GL_FRONT, GL_AMBIENT, metal_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, metal_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, metal_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, metal_shininess);

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

            // ��Ļ���ʣ��ǿ���Ч����
            GLfloat screen_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
            GLfloat screen_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT, screen_ambient);
            glMaterialfv(GL_FRONT, GL_SPECULAR, screen_specular);

            // Һ����Ļ����ƽ�ƺ����ţ�
            transMat1.SetTrans(CVector(0.0f, 0.03f, 0.1f));
            scaleMat1.SetScale(CVector(0.7f, 0.03f, 0.2f));
            glPushMatrix();
            glMultMatrixf(transMat1 * scaleMat1); // �ȼ�����������ƽ�Ƶ�����
            glColor3f(0.1f, 0.1f, 0.1f);
            glutSolidCube(1.0f);
            glPopMatrix();

            GLfloat button_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
            GLfloat button_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT, button_ambient);
            glMaterialfv(GL_FRONT, GL_SPECULAR, button_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

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

        glEnable(GL_LIGHT2);
        glEnable(GL_LIGHT3);
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
    glEnable(GL_LIGHT0);

    GLfloat glass_ambient[] = { 0.1f, 0.1f, 0.1f, 0.5f };
    GLfloat glass_diffuse[] = { 0.6f, 0.6f, 0.6f, 0.5f };
    GLfloat glass_specular[] = { 0.9f, 0.9f, 0.9f, 0.5f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, glass_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glass_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glass_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 96.0f);

    // �޸Ŀ�ʼ��ͷ��Բ׶��͸������
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT); // ����״̬
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6, 0.4, 0.7, 0.5); // ��4������0.5��ʾ50%͸����
    glutSolidCone(0.25f, 0.6f, 64, 64);
    glDisable(GL_BLEND);
    glPopAttrib(); // �ָ�ԭʼ״̬
    // �޸Ľ���

    glPopMatrix();

    glPopMatrix();

    // �װ� - �������
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myShip.floorTexture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // ���õذ����
    GLfloat floor_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat floor_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat floor_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    glPushMatrix();
    transMat1.SetTrans(CVector(0, -0.055, 0));
    scaleMat1.SetScale(CVector(0.475, 0.0002, 1.358));
    glMultMatrixf(transMat1* scaleMat1);

    // ���ƴ�����ͷ��ߵĵذ�
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // ��������
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_LIGHT0);
    glColor3f(1.0, 0.5, 0.0);// �������
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, -0.8f));
    glMultMatrixf(transMat1);
    glutSolidCone(0.2, 0.4, 8, 4);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);  // ��������
    glBindTexture(GL_TEXTURE_2D, myShip.wingTexture);  // �󶨻�������

    glEnable(GL_LIGHT0);

    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
    DrawWing(true);
    // ������Զ�����+���ߣ�
    glEnable(GL_LIGHT0);
    DrawWing(false);

    glDisable(GL_TEXTURE_2D);  // ��������

    glPopMatrix();

    glDisable(GL_LIGHTING);
}

// �ƾֲ�Y����ת��ƫ����
void ShipYaw(float angle) {
    CVector localUp = myShip.orientation * CVector(0, 1, 0); // ��ȡ�ֲ��Ϸ���
    CQuaternion rot;
    rot.SetAngle(angle, localUp);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

// �ƾֲ�X����ת��������
void ShipPitch(float angle) {
    CVector localRight = myShip.orientation * CVector(-1, 0, 0); // ��ȡ�ֲ��ҷ���
    CQuaternion rot;
    rot.SetAngle(angle, localRight);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

// �ƾֲ�Z����ת����ת��
void ShipRoll(float angle) {
    CVector localForward = myShip.orientation * CVector(0, 0, -1); // ��ȡ�ֲ�ǰ����
    CQuaternion rot;
    rot.SetAngle(angle, localForward);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

CVector getShipDir(ship myShip) {
    return (myShip.orientation * CVector(0, 0, 1)).Normalized();
}

void DrawWing(bool isRightWing) {
    glPushMatrix();

    // ============= �Զ������������� =============
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    // ���ݻ���ߴ����������������ɲ���
    GLfloat s_plane[4] = { 2.0f, 0.0f, 0.0f, 0.0f };
    GLfloat t_plane[4] = { 0.0f, 0.0f, 2.0f, 0.0f };
    glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane);

    // ============= �������� =============
    GLfloat metal_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat metal_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat metal_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, metal_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, metal_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, metal_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0f);

    // ============= ���α任 =============
    float wingSign = isRightWing ? 1.0f : -1.0f; // ���������

    // ����ʹ��glScalef���о��񣬶���ֱ������λ��
    transMat1.SetTrans(CVector(wingSign * 0.65f, 0, 0));
    scaleMat1.SetScale(CVector(0.8f, 0.1f, 0.5f));
    glMultMatrixf(transMat1 * scaleMat1);

    // ============= �����ߵ��ı��λ��� =============
    glBegin(GL_QUADS);

    // ǰ���棨������Z������
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // ����棨������Z������
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // �ϱ��棨������Y������
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // �±��棨������Y������
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // ����棨����������������߷���
    // ʹ��wingSignȷ������ʼ��ָ�����
    glNormal3f(wingSign, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    glEnd();

    // ============= ����״̬ =============
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();
}