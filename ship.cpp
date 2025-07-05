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
    astronautCamera.origonPos = astronautCamera.origonPos + myShip.speed;
    shipCamera.origonPos = shipCamera.origonPos + myShip.speed;
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

            astronautCamera.orientation = deltaQ * astronautCamera.orientation;
            astronautCamera.orientation.Normalize();
            astronautCamera.UpdateEulerFromOrientation();

            shipCamera.orientation = deltaQ * astronautCamera.orientation;
            shipCamera.orientation.Normalize();
            shipCamera.UpdateEulerFromOrientation();

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

        // 座垫部分
        scaleMat1.SetScale(CVector(0.5f, 0.1f, 0.2f));
        glPushMatrix();
        glMultMatrixf(scaleMat1);

        AABB cushionBox;
        cushionBox.partName = "Seat Cushion";
        cushionBox.min = CVector(-0.5f, -0.5f, -0.5f);
        cushionBox.max = CVector(0.5f, 0.5f, 0.5f);
        cushionBox.worldTransform = shipTransform * seatTransform * scaleMat1;
        myShip.collisionBoxes.push_back(cushionBox);
        DrawAABB(cushionBox, s);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.seatTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // 靠背部分
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
        DrawAABB(backrestBox, s);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.seatTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        GLfloat back_mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
        GLfloat back_mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, back_mat_ambient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, back_mat_specular);

        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

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

    myShip.seatTexture = LoadTexture("textures/seat.jpg");
    myShip.bodyTexture = LoadTexture("textures/ship_body.jpg");
    myShip.engineTexture = LoadTexture("textures/ship_body.jpg");
    myShip.wingTexture = LoadTexture("textures/ship_body.jpg");
    myShip.glassTexture = LoadTexture("textures/ship_body.jpg");
    myShip.panelTexture = LoadTexture("textures/ship_body.jpg");
    myShip.envMapTexture = LoadTexture("textures/ship_body.jpg");
    myShip.floorTexture = LoadTexture("textures/ship_body.jpg");
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
    DrawAABB(headMarkerBox, s);

    glutSolidSphere(0.2, 4, 4);
    glPopMatrix();

    // 船身主体
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

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.bodyTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glColor3f(1.0f, 1.0f, 1.0f);
        gluCylinder(quadric, 0.25f, 0.25f, 1.3f, 64, 64);

        GLfloat disk_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, disk_specular);
        glColor3f(0.8f, 0.8f, 0.8f);
        gluDisk(quadric, 0.0f, 0.25f, 48, 1);

        gluDeleteQuadric(quadric);
        glDisable(GL_TEXTURE_2D);

        DrawAABB(hullBox, s);
    }

    // 船头部分
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
    glutSolidCone(0.25f, 0.6f, 64, 64);
    glDisable(GL_BLEND);
    glPopAttrib();

    DrawAABB(noseBox, s);
    glPopMatrix();

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
            glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

            AABB consoleBaseBox;
            consoleBaseBox.partName = "Console Base";
            consoleBaseBox.min = CVector(-0.4f, -0.025f, -0.15f);
            consoleBaseBox.max = CVector(0.4f, 0.025f, 0.15f);
            CMatrix tempMat = finalMat * transMat2 * transMat3 * scaleMat2 * transMat1 * rotateMat1 * rotateMat2;
            consoleBaseBox.worldTransform = tempMat;
            myShip.collisionBoxes.push_back(consoleBaseBox);
            DrawAABB(consoleBaseBox, s);

            scaleMat1.SetScale(CVector(0.8f, 0.05f, 0.3f));
            glPushMatrix();
            glMultMatrixf(scaleMat1);
            glColor3f(0.2f, 0.2f, 0.2f);
            glutSolidCube(1.0f);
            glPopMatrix();

            GLfloat screen_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
            GLfloat screen_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT, screen_ambient);
            glMaterialfv(GL_FRONT, GL_SPECULAR, screen_specular);

            transMat1.SetTrans(CVector(0.0f, 0.03f, 0.1f));
            scaleMat1.SetScale(CVector(0.7f, 0.03f, 0.2f));
            glPushMatrix();
            glMultMatrixf(transMat1 * scaleMat1);

            AABB screenBox;
            screenBox.partName = "LCD Screen";
            screenBox.min = CVector(-0.5f, -0.5f, -0.5f);
            screenBox.max = CVector(0.5f, 0.5f, 0.5f);
            screenBox.worldTransform = tempMat * transMat1 * scaleMat1;
            myShip.collisionBoxes.push_back(screenBox);
            DrawAABB(screenBox, s);

            glColor3f(0.1f, 0.1f, 0.1f);
            glutSolidCube(1.0f);
            glPopMatrix();

            GLfloat button_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
            GLfloat button_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT, button_ambient);
            glMaterialfv(GL_FRONT, GL_SPECULAR, button_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

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

        // 使用修改后的drawSeat函数，传递飞船变换矩阵
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

    // 甲板
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myShip.floorTexture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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
    glMultMatrixf(transMat1 * scaleMat1);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_LIGHT0);
    glColor3f(1.0, 0.5, 0.0);
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, -0.8f));
    glMultMatrixf(transMat1);

    AABB engineBox;
    engineBox.partName = "Engine Nozzle";
    engineBox.min = CVector(-0.2f, -0.2f, 0.0f);
    engineBox.max = CVector(0.2f, 0.2f, 0.4f);
    engineBox.worldTransform = finalMat * transMat1;
    myShip.collisionBoxes.push_back(engineBox);
    DrawAABB(engineBox, s);

    glutSolidCone(0.2, 0.4, 8, 4);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myShip.wingTexture);

    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);

    // 使用修改后的DrawWing函数，传递飞船变换矩阵
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

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    GLfloat s_plane[4] = { 2.0f, 0.0f, 0.0f, 0.0f };
    GLfloat t_plane[4] = { 0.0f, 0.0f, 2.0f, 0.0f };
    glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane);

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

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glNormal3f(wingSign, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glEnd();

    DrawAABB(wingBox, s);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();
}