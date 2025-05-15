#include "Camera.h"
#include "ship.h"
#include "Astronaut.h"
#include <iostream>

extern Camera globalCamera, astronautCamera,tempCamera;
extern ship myShip;
extern Astronaut astronaut;
extern CVector g_lastCamPos;
extern CEuler g_lastCamEuler;
extern CVector g_lastCamForward;
extern CVector g_lastCamUp;
extern float g_lastCamSpeed;
extern Camera::ControlMode g_lastCamMode;

void Camera::StartTransitionTo(const Camera& target, float duration) {

    tempCamera = *this;
    transition.isActive = true;
    tempCamera.transition.startPos = this->position;
    tempCamera.transition.startOrientation = this->orientation;
    tempCamera.transition.targetCamera = &target;
    tempCamera.transition.duration = duration;
    tempCamera.transition.progress = 0.00f;
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
    position = position + GetUpDir() * moveSpeed;
}

void Camera::MoveDown() {
    position = position - GetUpDir() * moveSpeed;
}

void Camera::MoveLeft() {
    position = position - GetRightDir() * moveSpeed;
}

void Camera::MoveRight() {
    position = position + GetRightDir() * moveSpeed;
}

void Camera::MoveFront() {
    position = position + GetForwardDir() * moveSpeed;
}

void Camera::MoveBack() {
    position = position - GetForwardDir() * moveSpeed;
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
    glColor3f(0.9f, 0.0f, 0.9f); // ����ɫ
    GLint yPos = viewport[3] - 30; // �Ӷ�����ʼ
    char buffer[256];
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
    globalCamera.position = CVector(5, 5, 5);
    globalCamera.online = true;
    CEuler temp;
    temp.h = 45;
    temp.p = -35.264;
    temp.b = 0;
    temp.b = 0;
    globalCamera.eulerAngles = temp;
    globalCamera.UpdateOrientationFromEuler();
    globalCamera.SwitchControlMode(Camera::LOCAL);
    globalCamera.moveSpeed = 0.04f;

    astronautCamera.SwitchControlMode(Camera::LOCAL);
    astronautCamera.eulerAngles = CEuler(180, 0, 0); // ���� Z ��������
    astronautCamera.UpdateOrientationFromEuler();   // ͬ����Ԫ��

    g_lastCamPos = globalCamera.position;
    g_lastCamEuler = globalCamera.GetEulerAngles();
    g_lastCamForward = globalCamera.GetForwardDir();
    g_lastCamUp = globalCamera.GetUpDir();
    g_lastCamSpeed = globalCamera.moveSpeed;
    g_lastCamMode = globalCamera.currentMode;
}