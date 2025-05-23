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

    // 更新进度
    tempCamera.transition.progress += deltaTime / tempCamera.transition.duration;

    // 获取目标相机的当前状态
    CVector targetPos = tempCamera.transition.targetCamera->position;
    CQuaternion targetOrientation = tempCamera.transition.targetCamera->orientation;

    if (tempCamera.transition.progress >= 1.0f) {
        // 过渡完成
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
        // 线性插值位置
        float t = tempCamera.transition.progress;
        tempCamera.position = tempCamera.transition.startPos * (1 - t) + targetPos * t;

        // 球面线性插值朝向
        tempCamera.orientation = tempCamera.transition.startOrientation.Slerp(targetOrientation, t);
        UpdateEulerFromOrientation();
        return false;
    }
}

Camera::Camera()
    : position(0.0f, 0.0f, 0.0f),
    orientation(CQuaternion(1.0f, 0.0f, 0.0f, 0.0f)), // (w,x,y,z) 格式
    currentMode(EULER),
    rotateSpeed(1.0f) {
    eulerAngles = orientation.ToEuler();
    eulerAngles.Normal(); // 规范化欧拉角到标准范围
    }
    

/* 切换控制模式
 * newMode: 目标模式（EULER 或 LOCAL）
 * 切换时会自动同步两种表示方式的数据 */
void Camera::SwitchControlMode(ControlMode newMode) {
    if (newMode == currentMode) return;
    // 模式切换时的数据同步
    if (newMode == EULER) {
        // 从四元数获取欧拉角并规范化
        eulerAngles = orientation.ToEuler();
        eulerAngles.Normal();
    }
    else {
        // 从欧拉角生成四元数并归一化
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

/* 偏航旋转（绕垂直轴）
 * angleDelta: 角度变化量（度）
 * 欧拉模式：直接修改偏航角
 * 本地模式：绕当前上方向旋转 */
void Camera::RotateYaw(float angleDelta) {
    if (currentMode == EULER) {
        // 欧拉角模式直接累加角度
        eulerAngles.h += angleDelta * rotateSpeed;
        eulerAngles.Normal();
        UpdateOrientationFromEuler();
    }
    else {
        // 本地模式：绕当前上方向轴旋转
        CVector up = GetUpDir();
        CQuaternion rot;
        rot.SetAngle(angleDelta * rotateSpeed, up); // 角度转换为弧度
        orientation = rot * orientation; 
        orientation.Normalize();
    }
}

/* 俯仰旋转（绕右轴）
 * 欧拉模式：直接修改俯仰角
 * 本地模式：绕当前右方向轴旋转 */
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

/* 滚转旋转（绕前轴）
 * 欧拉模式：直接修改滚转角
 * 本地模式：绕当前前方向轴旋转 */
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

/* 获取欧拉角表示
 * 根据当前模式返回对应数据 */
CEuler Camera::GetEulerAngles() const {
    return (currentMode == EULER) ? eulerAngles : orientation.ToEuler();
}

/* 获取前方向向量
 * 从旋转矩阵的第三列（z轴方向）提取 */
CVector Camera::GetForwardDir() const {//初始（-1,-1,-1）
    CMatrix mat = orientation.ToMatrix();
    return CVector(-mat.m02, -mat.m12, -mat.m22).Normalized();
}

/* 获取上方向向量
 * 从旋转矩阵的第二列（y轴方向）提取 */
CVector Camera::GetUpDir() const {
    CMatrix mat = orientation.ToMatrix();
    return CVector(mat.m01, mat.m11, mat.m21).Normalized();
}

/* 获取右方向向量（私有辅助函数）
 * 从旋转矩阵的第一列（x轴方向）提取 */
CVector Camera::GetRightDir() const {
    CMatrix mat = orientation.ToMatrix();
    return CVector(mat.m00, mat.m10, mat.m20).Normalized();
}

/* 从欧拉角更新四元数朝向
 * 确保两种表示方式同步 */
void Camera::UpdateOrientationFromEuler() {
    orientation = eulerAngles.ToQuaternion();
    orientation.Normalize(); // 防止数值误差
}

/* 从四元数更新欧拉角
 * 用于模式切换时同步数据 */
void Camera::UpdateEulerFromOrientation() {
    eulerAngles = orientation.ToEuler();
    eulerAngles.Normal(); // 保持角度在合理范围
}

//状态更新函数
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
    GLint yPos = viewport[3] - 30; // 从顶部开始
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
    snprintf(buffer, sizeof(buffer), "Euler: H:%6.1f° P:%6.1f° B:%6.1f°",
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

    yPos -= 20;  // 继续下移20像素
    snprintf(buffer, sizeof(buffer), "Speed: %6.4f units/sec", moveSpeed);
    RenderString(10, yPos, buffer);
    yPos -= 20;  // 为后续可能添加的信息预留位置
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
    case LOCAL:  // 修改原来的QUATERNION为LOCAL
        return "Local"; // 对应LOCAL模式
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
    astronautCamera.eulerAngles = CEuler(180, 0, 0); // 朝向 Z 轴正方向
    astronautCamera.UpdateOrientationFromEuler();   // 同步四元数
    astronautCamera.origonPos = myShip.position;

    shipCamera.SwitchControlMode(Camera::EULER);
    shipCamera.eulerAngles = CEuler(180, 0, 0); // 朝向 Z 轴正方向
    shipCamera.UpdateOrientationFromEuler();   // 同步四元数
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
    const int rightColumnX = viewport[2] - 360; // 右侧起始X坐标

    //glColor3f(0.9f, 0.0f, 0.9f);
    
    GLint yPos = viewport[3] - 30;

    char buffer[256];
    if (strcmp(viewType, "Global") == 0) {
        glColor3f(0.0f, 0.9f, 0.9f); // 青色区分右侧列
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
        glColor3f(0.0f, 0.0f, 0.0f); // 青色区分右侧列
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