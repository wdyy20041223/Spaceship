#include "base.h"
#include "CMatrix.h"
#include "planet.h"

extern Camera globalCamera, astronautCamera, shipCamera;
extern bool ControllingShip;
char s = 's';

void shipMove() {
    myShip.direction = getShipDir(myShip);
    myShip.speed = myShip.direction * myShip.speedLen;
    myShip.position = myShip.position + myShip.speed;
}
CVector getForwardDirection(const CQuaternion& orientation) {
    CVector baseForward(0, 0, -1);
    return orientation * baseForward;
}
CVector getUpDirection(const CQuaternion& orientation) {
    CVector baseUp(0, 1, 0);
    return orientation * baseUp;
}
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
            CVector tempdir1 = myShip.direction.Normalized();
            CQuaternion q1 = tempdir1.ToEuler().ToQuaternion();
            CQuaternion q2 = finalDir.ToEuler().ToQuaternion();
            CQuaternion deltaQ = q2 * q1.Inverse();

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
    glLineWidth(2.0f);

    tempColor.Set(1.0f, 0.0f, 0.0f);
    glColor3fv(tempColor);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(4.5f, 0.0f, 0.0f);
    glEnd();

    glPushMatrix();
    transMat1.SetTrans(CVector(4.4, 0, 0));
    rotateMat1.SetRotate(90, CVector(0, 1, 0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();

    tempColor.Set(0.0f, 1.0f, 0.0f);
    glColor3fv(tempColor);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 3.0f, 0.0f);
    glEnd();

    glPushMatrix();
    transMat1.SetTrans(CVector(0, 2.9, 0));
    rotateMat1.SetRotate(-90, CVector(1, 0, 0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();

    tempColor.Set(0.0f, 0.0f, 1.0f);
    glColor3fv(tempColor);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 4.5f);
    glEnd();

    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, 4.4));
    glMultMatrixf(transMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();
}

void drawSeat(float len, float pos1, float pos2, const CMatrix& shipTransform) {
    glPushMatrix();
    {
        GLfloat mat_ambient[] = { 0.6f, 0.6f, 0.f, 1.0f };
        GLfloat mat_diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };
        GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_shininess = 80.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

        transMat1.SetTrans(CVector(pos1, -0.185f, pos2));
        rotateMat1.SetRotate(180.0f, CVector(0, 1, 0));
        scaleMat1.SetScale(CVector(len, 0.5f, 0.51f));

        CMatrix seatTransform = scaleMat1 * rotateMat1 * transMat1;
        glMultMatrixf(seatTransform);

        // ���沿��
        scaleMat1.SetScale(CVector(0.499f, 0.1f, 0.2f));
        glPushMatrix();
        glMultMatrixf(scaleMat1);

        AABB cushionBox;
        cushionBox.partName = "Seat Cushion";
        cushionBox.min = CVector(-0.5f, -0.5f, -0.5f);
        cushionBox.max = CVector(0.5f, 0.5f, 0.5f);
        cushionBox.worldTransform = shipTransform * seatTransform * scaleMat1;
        myShip.collisionBoxes.push_back(cushionBox);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.seatTexture);
        // �������������ֹ����
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // ��̬���������ظ�����
        float texRepeatX = len * 2.0f; // ������λ���ȶ�̬����X�����ظ�����

        glBegin(GL_QUADS);
        // ǰ����
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(texRepeatX, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(texRepeatX, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        // �����
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(texRepeatX, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(texRepeatX, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        // �ϱ���
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(texRepeatX, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(texRepeatX, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        // �±���
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(texRepeatX, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(texRepeatX, 1.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        // �����
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

        // �ұ���
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // ��������
        transMat1.SetTrans(CVector(0.0f, 0.1f, 0.1f));
        rotateMat1.SetRotate(10.0f, CVector(1, 0, 0));
        scaleMat1.SetScale(CVector(0.5f, 0.3f, 0.05f));
        glPushMatrix();
        glMultMatrixf(transMat1 * rotateMat1 * scaleMat1);

        AABB backrestBox;
        backrestBox.partName = "Seat Backrest";
        backrestBox.min = CVector(-0.5f, -0.5f, -0.5f);
        backrestBox.max = CVector(0.5f, 0.5f, 0.5f);
        backrestBox.worldTransform = shipTransform * seatTransform * transMat1 * rotateMat1 * scaleMat1;
        myShip.collisionBoxes.push_back(backrestBox);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.seatTexture);
        // �����������
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        float backrestTexRepeatX = len * 1.5f; // ��������X�����ظ�ϵ��

        GLfloat back_mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
        GLfloat back_mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, back_mat_ambient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, back_mat_specular);

        glBegin(GL_QUADS);
        // ǰ����
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(backrestTexRepeatX, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(backrestTexRepeatX, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        // �����
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(backrestTexRepeatX, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(backrestTexRepeatX, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        // �ϱ���
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(backrestTexRepeatX, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(backrestTexRepeatX, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        // �±���
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(backrestTexRepeatX, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(backrestTexRepeatX, 1.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        // �����
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

        // �ұ���
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

    myShip.seatTexture = LoadTexture("textures/ship_seat.png");
    myShip.bodyTexture = LoadTexture("textures/ship_body.png");
    myShip.engineTexture = LoadTexture("textures/ship_engine.png");
    myShip.wingTexture = LoadTexture("textures/ship_body.png");
    myShip.glassTexture = LoadTexture("textures/ship_body.jpg");
    myShip.floorTexture = LoadTexture("textures/ship_body.png");

    myShip.consoleTexture = LoadTexture("textures/ship_base.jpg");
    myShip.screenTexture = LoadTexture("textures/test.jpg");

}

void drawShip() {
    myShip.collisionBoxes.clear();

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);

    glPushMatrix();
    CMatrix rotateMat = myShip.orientation.ToMatrix();
    transMat1.SetTrans(myShip.position);
    scaleMat1.SetScale(CVector(0.2f, 0.2f, 0.2f));
    finalMat = transMat1 * rotateMat * scaleMat1;
    glMultMatrixf(finalMat);

    glColor3f(0.0, 0.0, 1.0);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0.35, 0));
    glMultMatrixf(transMat1);

    AABB headMarkerBox;
    headMarkerBox.partName = "Head Marker";
    headMarkerBox.min = CVector(-0.2, -0.2, -0.2);
    headMarkerBox.max = CVector(0.2, 0.2, 0.2);
    headMarkerBox.worldTransform = finalMat * transMat1;
    myShip.collisionBoxes.push_back(headMarkerBox);

    glutSolidSphere(0.2, 4, 4);
    glPopMatrix();

    // ��������
    glPushMatrix();
    {
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat mat_shininess = 128.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

        GLUquadricObj* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluQuadricNormals(quadric, GLU_SMOOTH);

        transMat2.SetTrans(CVector(0, 0, -0.65f));
        glMultMatrixf(transMat2);

        AABB hullBox;
        hullBox.partName = "Main Hull";
        hullBox.min = CVector(-0.25f, -0.25f, 0.0f);
        hullBox.max = CVector(0.25f, 0.25f, 1.3f);
        hullBox.worldTransform = finalMat * transMat2;
        myShip.collisionBoxes.push_back(hullBox);

        glPushMatrix();
        {
            // ����ڲ���ײ��
            transMat1.SetTrans(CVector(-0.259f, 0.0f, 0.68f)); // ��΢����ƫ��
            scaleMat1.SetScale(CVector(0.025f, 0.45f, 1.2f));   // ���ĳ�����
            glMultMatrixf(transMat1 * scaleMat1);

            AABB leftInnerBox;
            leftInnerBox.partName = "Ship Inner Body (Right)";
            leftInnerBox.min = CVector(-0.5f, -0.5f, -0.5f);
            leftInnerBox.max = CVector(0.5f, 0.5f, 0.5f);
            leftInnerBox.worldTransform = finalMat * transMat2 * transMat1 * scaleMat1;
            myShip.collisionBoxes.push_back(leftInnerBox);

        }
        glPopMatrix();

        glPushMatrix();
        {
            // �Ҳ��ڲ���ײ��
            transMat1.SetTrans(CVector(0.259f, 0.0f, 0.69f)); // ��΢����ƫ��
            scaleMat1.SetScale(CVector(0.025f, 0.45f, 1.2f));  // ���ĳ�����
            glMultMatrixf(transMat1 * scaleMat1);

            AABB rightInnerBox;
            rightInnerBox.partName = "Ship Inner Body (Left)";
            rightInnerBox.min = CVector(-0.5f, -0.5f, -0.5f);
            rightInnerBox.max = CVector(0.5f, 0.5f, 0.5f);
            rightInnerBox.worldTransform = finalMat * transMat2 * transMat1 * scaleMat1;
            myShip.collisionBoxes.push_back(rightInnerBox);

            // ����Ҫʵ�ʻ��ƣ���������ײ���
        }
        glPopMatrix();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.bodyTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // �����������
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // ���������ظ�����
        float cylinderHeight = 1.3f;
        float cylinderCircumference = 2 * 3.14159f * 0.25f;
        float texRepeatY = cylinderHeight / cylinderCircumference;

        // Ӧ������任
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        CMatrix t1;
        t1.SetScale(CVector(4.0f, texRepeatY * 4.0f, 1.0f));
        glMultMatrixf(t1);

        gluCylinder(quadric, 0.25f, 0.25f, cylinderHeight, 64, 64);

        // �ָ��������
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        GLfloat disk_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, disk_specular);
        glColor3f(0.8f, 0.8f, 0.8f);
        gluDisk(quadric, 0.0f, 0.25f, 48, 1);

        gluDeleteQuadric(quadric);
        glDisable(GL_TEXTURE_2D);
    }

    // ��ͷ����
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, 1.3f));
    glMultMatrixf(transMat1);

    AABB noseBox;
    noseBox.partName = "Ship Nose";
    noseBox.min = CVector(-0.25f, -0.25f, 0.0f);
    noseBox.max = CVector(0.25f, 0.25f, 0.6f);
    noseBox.worldTransform = finalMat * transMat2 * transMat1;
    myShip.collisionBoxes.push_back(noseBox);

    GLfloat glass_ambient[] = { 0.1f, 0.1f, 0.1f, 0.5f };
    GLfloat glass_diffuse[] = { 0.6f, 0.6f, 0.6f, 0.5f };
    GLfloat glass_specular[] = { 0.9f, 0.9f, 0.9f, 0.5f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, glass_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glass_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glass_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 96.0f);

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6, 0.4, 0.7, 0.5);
    glutSolidCone(0.25f, 0.6f, 64, 16);
    glDisable(GL_BLEND);
    glPopAttrib();

    glPopMatrix();


    //����̨
    glPushMatrix();
    CMatrix transMat3;
    transMat3.SetTrans(CVector(0, 0, 1.3f));
    glMultMatrixf(transMat3);
    glPushMatrix();
    {
        scaleMat2.SetScale(CVector(0.5f, 0.5f, 0.5f));
        glMultMatrixf(scaleMat2);

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

            transMat1.SetTrans(CVector(0.0f, -0.105f, -0.4f));
            rotateMat1.SetRotate(180.0f, CVector(0, 1, 0));
            rotateMat2.SetRotate(5.0f, CVector(1, 0, 0));
            glMultMatrixf(transMat1* rotateMat1* rotateMat2);

            AABB consoleBaseBox;
            consoleBaseBox.partName = "Console Base";
            consoleBaseBox.min = CVector(-0.4f, -0.025f, -0.15f);
            consoleBaseBox.max = CVector(0.4f, 0.025f, 0.15f);
            CMatrix tempMat = finalMat * transMat2 * transMat3 * scaleMat2 * transMat1 * rotateMat1 * rotateMat2;
            consoleBaseBox.worldTransform = tempMat;
            myShip.collisionBoxes.push_back(consoleBaseBox);

            // ����̨�����������
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, myShip.consoleTexture);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // ������������

            // �����������
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            scaleMat1.SetScale(CVector(0.8f, 0.05f, 0.3f));
            glPushMatrix();
            glMultMatrixf(scaleMat1);

            // �ֶ����ƴ������������
            float texScale = 2.0f; // �������ű���
            glBegin(GL_QUADS);

            // ����
            glNormal3f(0, 1, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
            glTexCoord2f(0.0f, texScale); glVertex3f(-0.5f, 0.5f, 0.5f);
            glTexCoord2f(texScale, texScale); glVertex3f(0.5f, 0.5f, 0.5f);
            glTexCoord2f(texScale, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);

            // ǰ��
            glNormal3f(0, 0, 1);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
            glTexCoord2f(texScale, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
            glTexCoord2f(texScale, texScale); glVertex3f(0.5f, 0.5f, 0.5f);
            glTexCoord2f(0.0f, texScale); glVertex3f(-0.5f, 0.5f, 0.5f);

            // ����
            glNormal3f(0, 0, -1);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2f(0.0f, texScale); glVertex3f(-0.5f, 0.5f, -0.5f);
            glTexCoord2f(texScale, texScale); glVertex3f(0.5f, 0.5f, -0.5f);
            glTexCoord2f(texScale, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

            // ����
            glNormal3f(-1, 0, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2f(texScale, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
            glTexCoord2f(texScale, texScale); glVertex3f(-0.5f, 0.5f, 0.5f);
            glTexCoord2f(0.0f, texScale); glVertex3f(-0.5f, 0.5f, -0.5f);

            // ����
            glNormal3f(1, 0, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
            glTexCoord2f(texScale, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
            glTexCoord2f(texScale, texScale); glVertex3f(0.5f, 0.5f, 0.5f);
            glTexCoord2f(0.0f, texScale); glVertex3f(0.5f, -0.5f, 0.5f);
            glEnd();
            glPopMatrix();
            glDisable(GL_TEXTURE_2D);

            // ��Ļ��������
            GLfloat screen_ambient[] = { 0.1f, 0.1f, 0.1f, 0.7f }; // ����alphaֵ
            GLfloat screen_specular[] = { 0.4f, 0.4f, 0.4f, 0.7f }; // ����alphaֵ
            glMaterialfv(GL_FRONT, GL_AMBIENT, screen_ambient);
            glMaterialfv(GL_FRONT, GL_SPECULAR, screen_specular);

            transMat1.SetTrans(CVector(0.0f, 0.03f, 0.1f));
            scaleMat1.SetScale(CVector(0.7f, 0.03f, 0.2f));
            glPushMatrix();
            glMultMatrixf(transMat1* scaleMat1);

            AABB screenBox;
            screenBox.partName = "LCD Screen";
            screenBox.min = CVector(-0.5f, -0.5f, -0.5f);
            screenBox.max = CVector(0.5f, 0.5f, 0.5f);
            screenBox.worldTransform = tempMat * transMat1 * scaleMat1;
            myShip.collisionBoxes.push_back(screenBox);

            // ���û����ʵ�ְ�͸��Ч��
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // ��Ļ������� - ����ʵ��
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, myShip.screenTexture);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // ��ΪMODULATE��֧�ְ�͸��

            // �����������
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // ���������ظ�����������ʵ�ʳߴ磩
            float temp = 4;
            float texRepeatFront = 1.0f * temp; // ���������ظ�����
            float texRepeatSide = 0.5f * temp;   // ���������ظ�����
            float texRepeatTop = 0.2f * temp;   // ���������ظ�����

            // ������Ļ������������棨����͸�Ӵ���
            glBegin(GL_QUADS);
            // ���棨��Ļ��ʾ�棩
            glNormal3f(0, 0, 1);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f); // ����
            glTexCoord2f(texRepeatFront, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);  // ����
            glTexCoord2f(texRepeatFront, texRepeatFront); glVertex3f(0.5f, 0.5f, 0.5f);  // ����
            glTexCoord2f(0.0f, texRepeatFront); glVertex3f(-0.5f, 0.5f, 0.5f);  // ����

            // ���棨ȷ�����ῴ���ڲ���
            glNormal3f(0, 0, -1);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2f(0.0f, texRepeatFront); glVertex3f(-0.5f, 0.5f, -0.5f);
            glTexCoord2f(texRepeatFront, texRepeatFront); glVertex3f(0.5f, 0.5f, -0.5f);
            glTexCoord2f(texRepeatFront, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

            // ����
            glNormal3f(0, 1, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
            glTexCoord2f(0.0f, texRepeatTop); glVertex3f(-0.5f, 0.5f, 0.5f);
            glTexCoord2f(texRepeatFront, texRepeatTop); glVertex3f(0.5f, 0.5f, 0.5f);
            glTexCoord2f(texRepeatFront, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);

            // ����
            glNormal3f(0, -1, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2f(texRepeatFront, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
            glTexCoord2f(texRepeatFront, texRepeatTop); glVertex3f(0.5f, -0.5f, 0.5f);
            glTexCoord2f(0.0f, texRepeatTop); glVertex3f(-0.5f, -0.5f, 0.5f);

            // �����
            glNormal3f(-1, 0, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2f(texRepeatSide, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
            glTexCoord2f(texRepeatSide, texRepeatFront); glVertex3f(-0.5f, 0.5f, 0.5f);
            glTexCoord2f(0.0f, texRepeatFront); glVertex3f(-0.5f, 0.5f, -0.5f);

            // �Ҳ���
            glNormal3f(1, 0, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
            glTexCoord2f(texRepeatSide, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
            glTexCoord2f(texRepeatSide, texRepeatFront); glVertex3f(0.5f, 0.5f, 0.5f);
            glTexCoord2f(0.0f, texRepeatFront); glVertex3f(0.5f, 0.5f, -0.5f);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND); // ���û��
            glPopMatrix();

            // ���ư�ť������ԭ����
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

        // ʹ���޸ĺ��drawSeat���������ݷɴ��任����
        CMatrix temp = finalMat * transMat2 * transMat3 * scaleMat2;
        drawSeat(0.5, 0, 1.5, temp);
        drawSeat(1.91, 0, 4, temp);
        drawSeat(0.5f, -0.7, 2, temp);
        drawSeat(0.5f, -0.7, 2.5, temp);
        drawSeat(0.5f, -0.7, 3, temp);
        drawSeat(0.5f, -0.7, 3.5, temp);
        drawSeat(0.5f, 0.7, 2, temp);
        drawSeat(0.5f, 0.7, 2.5, temp);
        drawSeat(0.5f, 0.7, 3, temp);
        drawSeat(0.5f, 0.7, 3.5, temp);
    }
    glPopMatrix();
    glEnable(GL_LIGHT0);

    glPopMatrix();

    glPopMatrix();

    // �װ�
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myShip.floorTexture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // �����������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLfloat floor_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat floor_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat floor_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    glPushMatrix();
    transMat1.SetTrans(CVector(0, -0.055, 0.01));
    scaleMat1.SetScale(CVector(0.485, 0.0002, 1.278));
    glMultMatrixf(transMat1* scaleMat1);

    // ���������ظ�����������ʵ�ʳߴ������
    float texRepeatX = 4.0f; // X�����ظ�4��
    float texRepeatZ = 1.358f / 0.475f * 4.0f; // Z���򰴱����ظ�[3](@ref)

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(texRepeatX, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(texRepeatX, texRepeatZ); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, texRepeatZ); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //����

    glEnable(GL_LIGHT0);
    // ���沿�֣��������
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myShip.engineTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLfloat engine_ambient[] = { 0.7f, 0.5f, 0.2f, 1.0f };
    GLfloat engine_diffuse[] = { 1.0f, 0.7f, 0.3f, 1.0f };
    GLfloat engine_specular[] = { 1.0f, 0.9f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, engine_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, engine_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, engine_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);

    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, -0.8f));
    glMultMatrixf(transMat1);

    // ʹ��UVӳ���������Բ׶��
    GLUquadricObj* engineQuadric = gluNewQuadric();
    gluQuadricTexture(engineQuadric, GL_TRUE);
    gluQuadricNormals(engineQuadric, GLU_SMOOTH);
    gluCylinder(engineQuadric, 0.2f, 0.0f, 0.4f, 16, 4); // Բ׶������
    gluDeleteQuadric(engineQuadric);

    // ��������ڲ�����Ч�����ٶ������ɫ��
    glPushAttrib(GL_ALL_ATTRIB_BITS); // ��������OpenGL״̬
    glDisable(GL_LIGHTING);           // �������й��ռ���
    glDisable(GL_TEXTURE_2D);         // ��������
    glDisable(GL_BLEND);              // ���û�ϣ�������Ҫ��͸����
    glDisable(GL_FOG);                // ������Ч
    glDepthMask(GL_FALSE);            // �������д�루��ֹ��ȳ�ͻ��
    glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
    glEnable(GL_COLOR_MATERIAL);
    float speed = myShip.speedLen;
    float intensity = fmin(speed / 0.025f, 1.0f); // 0��0, 0.025��1.0
    float glow = pow(intensity, 2.0f); // ���η�������ǿ����
    glColor3f(glow, 0.0f, 0.0f); // RGBģʽ������
    // ���Ʒ������
    glutSolidSphere(0.15f, 24, 24);
    glPopAttrib(); // �ָ�ԭʼOpenGL״̬


    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myShip.wingTexture);

    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);

    // ʹ���޸ĺ��DrawWing���������ݷɴ��任����
    DrawWing(true, finalMat);
    glEnable(GL_LIGHT0);
    DrawWing(false, finalMat);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glDisable(GL_LIGHTING);
}

void ShipYaw(float angle) {
    CVector localUp = myShip.orientation * CVector(0, 1, 0);
    CQuaternion rot;
    rot.SetAngle(angle, localUp);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

void ShipPitch(float angle) {
    CVector localRight = myShip.orientation * CVector(-1, 0, 0);
    CQuaternion rot;
    rot.SetAngle(angle, localRight);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

void ShipRoll(float angle) {
    CVector localForward = myShip.orientation * CVector(0, 0, -1);
    CQuaternion rot;
    rot.SetAngle(angle, localForward);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

CVector getShipDir(ship myShip) {
    return (myShip.orientation * CVector(0, 0, 1)).Normalized();
}

void DrawWing(bool isRightWing, const CMatrix& shipTransform) {
    glPushMatrix();

    // ���������Զ����ɣ��ؼ��޸ģ�
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myShip.wingTexture);

    GLfloat metal_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat metal_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat metal_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, metal_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, metal_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, metal_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0f);

    float wingSign = isRightWing ? 1.0f : -1.0f;
    transMat1.SetTrans(CVector(wingSign * 0.65f, 0, 0));
    scaleMat1.SetScale(CVector(0.8f, 0.1f, 0.5f));
    CMatrix wingTransform = transMat1 * scaleMat1;
    glMultMatrixf(wingTransform);

    AABB wingBox;
    wingBox.partName = isRightWing ? "Right Wing" : "Left Wing";
    wingBox.min = CVector(-0.5f, -0.5f, -0.5f);
    wingBox.max = CVector(0.5f, 0.5f, 0.5f);
    wingBox.worldTransform = shipTransform * wingTransform;
    myShip.collisionBoxes.push_back(wingBox);

    // �����ظ����������ڻ���ʵ�ʳߴ磩
    const float texScaleX = 2.0f; // X�����ظ���
    const float texScaleY = 0.5f; // Y�����ظ���
    const float texScaleZ = 1.0f; // Z�����ظ���

    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);                glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(texScaleX, 0.0f);           glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(texScaleX, texScaleY);       glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, texScaleY);            glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);                glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, texScaleY);            glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(texScaleX, texScaleY);       glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(texScaleX, 0.0f);           glVertex3f(0.5f, -0.5f, -0.5f);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);                glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, texScaleZ);            glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(texScaleX, texScaleZ);       glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(texScaleX, 0.0f);            glVertex3f(0.5f, 0.5f, -0.5f);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);                glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(texScaleX, 0.0f);            glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(texScaleX, texScaleZ);       glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, texScaleZ);            glVertex3f(-0.5f, -0.5f, 0.5f);


    // ����ⲿ���棨����������棩
    glNormal3f(-wingSign, 0.0f, 0.0f);
    if (isRightWing) {
        // Right wing outer surface
        glTexCoord2f(0.0f, 0.0f);            glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(0.0f, texScaleY);        glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(texScaleX, texScaleY);   glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(texScaleX, 0.0f);       glVertex3f(0.5f, -0.5f, -0.5f);
    }
    else {
        // Left wing outer surface
        glTexCoord2f(0.0f, 0.0f);            glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(texScaleX, 0.0f);       glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(texScaleX, texScaleY);   glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, texScaleY);        glVertex3f(-0.5f, 0.5f, 0.5f);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}