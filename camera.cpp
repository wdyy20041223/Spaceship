#include "Camera.h"
#include "ship.h"
#include "Astronaut.h"
#include <iostream>
#include <GL/glut.h>

extern Camera globalCamera, astronautCamera,tempCamera, shipCamera,planetCamera;
extern ship myShip;
extern Astronaut astronaut;
extern CVector g_lastCamPos;
extern CEuler g_lastCamEuler;
extern CVector g_lastCamForward;
extern CVector g_lastCamUp;
extern float g_lastCamSpeed;
extern Camera::ControlMode g_lastCamMode;
extern bool ControlingGlobal,needGuide;

void Camera::StartTransitionTo(Camera& target, float duration) {

    tempCamera = *this;
    transition.isActive = true;
    tempCamera.transition.startPos = this->position;
    tempCamera.transition.startOrientation = this->orientation;
    tempCamera.transition.targetCamera = &target;
    tempCamera.transition.duration = duration;
    tempCamera.transition.progress = 0.00f;
    transition.targetCamera = &target;
}

bool Camera::UpdateTransition(float deltaTime) {
    if (!transition.isActive) return false;

    // ���½���
    tempCamera.transition.progress += deltaTime / tempCamera.transition.duration;

    // ��ȡĿ������ĵ�ǰ״̬
    CVector targetPos = tempCamera.transition.targetCamera->position;
    CQuaternion targetOrientation = tempCamera.transition.targetCamera->orientation;

    if (tempCamera.transition.progress >= 1.0f) {
        // �������
        globalCamera.online = false;
        astronautCamera.online = false;
        shipCamera.online = false;
        transition.targetCamera->online = true;

        if (transition.targetCamera == &planetCamera) {
            globalCamera.online = true;
            globalCamera.origonPos = planetCamera.origonPos;
            globalCamera.position = CVector(0, 0, 0);
            globalCamera.orientation = planetCamera.orientation;
        }

        if (tempCamera.transition.targetCamera == &globalCamera|| tempCamera.transition.targetCamera == &planetCamera) {
            ControlingGlobal = true;
        }
        else {
            ControlingGlobal = false;
        }

        transition.isActive = false;
        return true;
    }
    else {
        // ���Բ�ֵλ��
        float t = tempCamera.transition.progress;
        tempCamera.position = tempCamera.transition.startPos * (1 - t) + targetPos * t;

        // �������Բ�ֵ����
        tempCamera.orientation = tempCamera.transition.startOrientation.Slerp(targetOrientation, t);
        UpdateEulerFromOrientation();
        return false;
    }
}

Camera::Camera()
    : position(0.0f, 0.0f, 0.0f),
    orientation(CQuaternion(1.0f, 0.0f, 0.0f, 0.0f)), // (w,x,y,z) ��ʽ
    currentMode(EULER),
    rotateSpeed(1.0f) {
    eulerAngles = orientation.ToEuler();
    eulerAngles.Normal(); // �淶��ŷ���ǵ���׼��Χ
    }
    

/* �л�����ģʽ
 * newMode: Ŀ��ģʽ��EULER �� LOCAL��
 * �л�ʱ���Զ�ͬ�����ֱ�ʾ��ʽ������ */
void Camera::SwitchControlMode(ControlMode newMode) {
    if (newMode == currentMode) return;
    // ģʽ�л�ʱ������ͬ��
    if (newMode == EULER) {
        // ����Ԫ����ȡŷ���ǲ��淶��
        eulerAngles = orientation.ToEuler();
        eulerAngles.Normal();
    }
    else {
        // ��ŷ����������Ԫ������һ��
        orientation = eulerAngles.ToQuaternion();
        orientation.Normalize();
    }
    currentMode = newMode;
}

void Camera::MoveUp() {
    origonPos = origonPos + GetUpDir() * speedLen;

}

void Camera::MoveDown() {
    origonPos = origonPos - GetUpDir() * speedLen;

}

void Camera::MoveLeft() {
    origonPos = origonPos - GetRightDir() * speedLen;

}

void Camera::MoveRight() {
    origonPos = origonPos + GetRightDir() * speedLen;

}

void Camera::MoveFront() {
    origonPos = origonPos + GetForwardDir() * speedLen;

}

void Camera::MoveBack() {
    origonPos = origonPos - GetForwardDir() * speedLen;

}

/* ƫ����ת���ƴ�ֱ�ᣩ
 * angleDelta: �Ƕȱ仯�����ȣ�
 * ŷ��ģʽ��ֱ���޸�ƫ����
 * ����ģʽ���Ƶ�ǰ�Ϸ�����ת */
void Camera::RotateYaw(float angleDelta) {
    if (currentMode == EULER) {
        // ŷ����ģʽֱ���ۼӽǶ�
        eulerAngles.h += angleDelta * rotateSpeed;
        eulerAngles.Normal();
        UpdateOrientationFromEuler();
    }
    else {
        // ����ģʽ���Ƶ�ǰ�Ϸ�������ת
        CVector up = GetUpDir();
        CQuaternion rot;
        rot.SetAngle(angleDelta * rotateSpeed, up); // �Ƕ�ת��Ϊ����
        orientation = rot * orientation; 
        orientation.Normalize();
    }
}

/* ������ת�������ᣩ
 * ŷ��ģʽ��ֱ���޸ĸ�����
 * ����ģʽ���Ƶ�ǰ�ҷ�������ת */
void Camera::RotatePitch(float angleDelta) {
    if (currentMode == EULER) {
        eulerAngles.p += angleDelta * rotateSpeed;
        eulerAngles.Normal();
        UpdateOrientationFromEuler();
    }
    else {
        CVector right = GetRightDir();
        CQuaternion rot;
        rot.SetAngle(angleDelta * rotateSpeed, right);
        orientation = rot * orientation;
        orientation.Normalize();
    }
}

/* ��ת��ת����ǰ�ᣩ
 * ŷ��ģʽ��ֱ���޸Ĺ�ת��
 * ����ģʽ���Ƶ�ǰǰ��������ת */
void Camera::RotateRoll(float angleDelta) {
    if (currentMode == EULER) {
        eulerAngles.b -= angleDelta * rotateSpeed;
        eulerAngles.Normal();
        UpdateOrientationFromEuler();
    }
    else {
        CVector forward = GetForwardDir();
        CQuaternion rot;
        rot.SetAngle(angleDelta * rotateSpeed, forward);
        orientation = rot * orientation; 
        orientation.Normalize();
    }
}

/* ��ȡŷ���Ǳ�ʾ
 * ���ݵ�ǰģʽ���ض�Ӧ���� */
CEuler Camera::GetEulerAngles() const {
    return (currentMode == EULER) ? eulerAngles : orientation.ToEuler();
}

/* ��ȡǰ��������
 * ����ת����ĵ����У�z�᷽����ȡ */
CVector Camera::GetForwardDir() const {//��ʼ��-1,-1,-1��
    CMatrix mat = orientation.ToMatrix();
    return CVector(-mat.m02, -mat.m12, -mat.m22).Normalized();
}

/* ��ȡ�Ϸ�������
 * ����ת����ĵڶ��У�y�᷽����ȡ */
CVector Camera::GetUpDir() const {
    CMatrix mat = orientation.ToMatrix();
    return CVector(mat.m01, mat.m11, mat.m21).Normalized();
}

/* ��ȡ�ҷ���������˽�и���������
 * ����ת����ĵ�һ�У�x�᷽����ȡ */
CVector Camera::GetRightDir() const {
    CMatrix mat = orientation.ToMatrix();
    return CVector(mat.m00, mat.m10, mat.m20).Normalized();
}

/* ��ŷ���Ǹ�����Ԫ������
 * ȷ�����ֱ�ʾ��ʽͬ�� */
void Camera::UpdateOrientationFromEuler() {
    orientation = eulerAngles.ToQuaternion();
    orientation.Normalize(); // ��ֹ��ֵ���
}

/* ����Ԫ������ŷ����
 * ����ģʽ�л�ʱͬ������ */
void Camera::UpdateEulerFromOrientation() {
    eulerAngles = orientation.ToEuler();
    eulerAngles.Normal(); // ���ֽǶ��ں���Χ
}

//״̬���º���
void Camera::Update() {
    if (currentMode == EULER) {
        UpdateOrientationFromEuler();
    }
    else {
        UpdateEulerFromOrientation();
    }
}


void Camera::RenderInfo(const char* viewType) const {
    GLboolean depthTestEnabled;
    GLint oldMatrixMode;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    glGetIntegerv(GL_MATRIX_MODE, &oldMatrixMode);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluOrtho2D(0, viewport[2], 0, viewport[3]);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(0.9f, 0.0f, 0.9f); 
    GLint yPos = viewport[3] - 30; // �Ӷ�����ʼ
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "--------Camera Info--------");
    RenderString(10, yPos, buffer);
    yPos -= 20;
    snprintf(buffer, sizeof(buffer), "View: %s", viewType);
    RenderString(10, yPos, buffer);
    yPos -= 20;
    snprintf(buffer, sizeof(buffer), "Control Mode: %s", GetControlModeString());
    RenderString(10, yPos, buffer);
    yPos -= 20;
    snprintf(buffer, sizeof(buffer), "Position: (%.2f, %.2f, %.2f)",
        position.x, position.y, position.z);
    RenderString(10, yPos, buffer);
    yPos -= 20;
    CEuler euler = GetEulerAngles();
    snprintf(buffer, sizeof(buffer), "Euler: H:%6.1f�� P:%6.1f�� B:%6.1f��",
        euler.h, euler.p, euler.b);
    RenderString(10, yPos, buffer);
    yPos -= 20;
    CVector forward = GetForwardDir();
    snprintf(buffer, sizeof(buffer), "Forward: (%5.2f, %5.2f, %5.2f)",
        forward.x, forward.y, forward.z);
    RenderString(10, yPos, buffer);
    yPos -= 20;
    CVector up = GetUpDir();
    snprintf(buffer, sizeof(buffer), "Up: (%5.2f, %5.2f, %5.2f)",
        up.x, up.y, up.z);
    RenderString(10, yPos, buffer);

    yPos -= 20;  // ��������20����
    snprintf(buffer, sizeof(buffer), "Speed: %6.4f units/sec", moveSpeed);
    RenderString(10, yPos, buffer);
    yPos -= 20;  // Ϊ����������ӵ���ϢԤ��λ��
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(oldMatrixMode);

    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Camera::RenderString(int x, int y, const char* str) const {
    glRasterPos2i(x, y);
    for (const char* c = str; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
}

const char* Camera::GetControlModeString() const {
    switch (currentMode) {
    case EULER:
        return "Euler";
    case LOCAL:  // �޸�ԭ����QUATERNIONΪLOCAL
        return "Local"; // ��ӦLOCALģʽ
    default:
        return "Unknown";
    }
}

void initCamera() {
    globalCamera.origonPos = CVector(5, 5, 5);

    globalCamera.online = true;
    astronautCamera.online = false;
    shipCamera.online = false;

    globalCamera.transition.isActive = false;
    astronautCamera.transition.isActive = false;
    shipCamera.transition.isActive = false;

    CEuler temp;
    temp.h = 45;
    temp.p = -35.264;
    temp.b = 0;
    temp.b = 0;
    globalCamera.eulerAngles = temp;
    globalCamera.UpdateOrientationFromEuler();
    globalCamera.SwitchControlMode(Camera::LOCAL);

    globalCamera.speedLen = 0.04f;
    astronautCamera.speedLen = 0.001f;
    shipCamera.speedLen = 0.001f;

    astronautCamera.SwitchControlMode(Camera::EULER);
    astronautCamera.eulerAngles = CEuler(180, 0, 0); // ���� Z ��������
    astronautCamera.UpdateOrientationFromEuler();   // ͬ����Ԫ��
    astronautCamera.origonPos = myShip.position;

    shipCamera.SwitchControlMode(Camera::EULER);
    shipCamera.eulerAngles = CEuler(180, 0, 0); // ���� Z ��������
    shipCamera.UpdateOrientationFromEuler();   // ͬ����Ԫ��
    shipCamera.origonPos = myShip.position;


    g_lastCamPos = globalCamera.position;
    g_lastCamEuler = globalCamera.GetEulerAngles();
    g_lastCamForward = globalCamera.GetForwardDir();
    g_lastCamUp = globalCamera.GetUpDir();
    g_lastCamSpeed = globalCamera.moveSpeed;
    g_lastCamMode = globalCamera.currentMode;


}

void Camera::OptionInfo(const char* viewType) const {
    if (!needGuide) {
        return;
    }
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const int rightColumnX = viewport[2] - 360; // �Ҳ���ʼX����

    //glColor3f(0.9f, 0.0f, 0.9f);
    
    GLint yPos = viewport[3] - 30;

    char buffer[256];
    if (strcmp(viewType, "Global") == 0) {
        glColor3f(0.0f, 0.9f, 0.9f); // ��ɫ�����Ҳ���
        snprintf(buffer, sizeof(buffer), "-------------Option Guide-------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "View: %s Camera", viewType);
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'m'to hide/open this option guide");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Switch Camera-------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'o' to Astronaut Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'p' to ship Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Control Camera------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'c' to switch Euler/Local");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'t' to turn up Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'g' to turn down Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'f' to turn left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'h' to turn right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'r' to roll left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'y' to roll right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F2' to switch move camera/coltrol ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'w' to move up Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'s' to move down Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'a' to move left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'d' to move right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'q' to move forward Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'e' to move back Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Control Ship--------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F2' to switch control ship/move camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'w' to turn up Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'s' to turn down Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'a' to turn left Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'d' to turn right Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'q' to roll left Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'e' to roll right Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'u' to speed up");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'j' to speed down");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Select Planet-------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "mouse click to select planet");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "ENTER to start/end self-navigation");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'l' to go to the planet's surface");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Switch Draw Mode-----------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F1' switch draw mode");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
    }
    else if (strcmp(viewType, "Astronaut") == 0) {
        glColor3f(0.0f, 0.0f, 0.0f); // ��ɫ�����Ҳ���
        snprintf(buffer, sizeof(buffer), "-------------Option Guide-------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "View: %s Camera", viewType);
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'m'to hide/open this option guide");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Switch Camera-------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'i' to Global Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'p' to ship Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Control Camera------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'c' to switch Euler/Local");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'t' to turn up Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'g' to turn down Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'f' to turn left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'h' to turn right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'r' to roll left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'y' to roll right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F2' to switch move camera/astronaut");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'w' to move up Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'s' to move down Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'a' to move left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'d' to move right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'q' to move forward Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'e' to move back Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Control Astronaut---------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F2' to switch move astronaut/camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'w' to look up");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'s' to look down");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'a' to look left");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'d' to look right");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'q' to move forward");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'e' to move back");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Switch Draw Mode-----------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F1' switch draw mode");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
    }
    else {
        glColor3f(0.0f, 0.9f, 0.9f); 
        snprintf(buffer, sizeof(buffer), "-------------Option Guide-------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "View: %s Camera", viewType);
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'m'to hide/open this option guide");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Switch Camera-------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'i' to Global Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'p' to ship Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Control Camera------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'c' to switch Euler/Local");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'t' to turn up Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'g' to turn down Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'f' to turn left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'h' to turn right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'r' to roll left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'y' to roll right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F2' to switch control ship/move camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'w' to move up Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'s' to move down Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'a' to move left Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'d' to move right Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'q' to move forward Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'e' to move back Camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Control Ship--------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F2' to switch control ship/move camera");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'w' to turn up Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'s' to turn down Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'a' to turn left Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'d' to turn right Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'q' to roll left Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'e' to roll right Ship");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'u' to speed up");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'j' to speed down");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Select Planet-------------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "mouse click to select planet");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "ENTER to start/end self-navigation");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "Switch Draw Mode-----------------------");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
        snprintf(buffer, sizeof(buffer), "'F1' switch draw mode");
        RenderString(rightColumnX, yPos, buffer);
        yPos -= 20;
    }

    glPopMatrix();
}