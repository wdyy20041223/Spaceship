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

    // 更新进度
    tempCamera.transition.progress += deltaTime / tempCamera.transition.duration;

    // 获取目标相机的当前状态
    CVector targetPos = tempCamera.transition.targetCamera->position;
    CQuaternion targetOrientation = tempCamera.transition.targetCamera->orientation;

    if (tempCamera.transition.progress >= 1.0f) {
        // 过渡完成
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
    glColor3f(0.9f, 0.0f, 0.9f); // 亮绿色
    GLint yPos = viewport[3] - 30; // 从顶部开始
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
    astronautCamera.eulerAngles = CEuler(180, 0, 0); // 朝向 Z 轴正方向
    astronautCamera.UpdateOrientationFromEuler();   // 同步四元数

    g_lastCamPos = globalCamera.position;
    g_lastCamEuler = globalCamera.GetEulerAngles();
    g_lastCamForward = globalCamera.GetForwardDir();
    g_lastCamUp = globalCamera.GetUpDir();
    g_lastCamSpeed = globalCamera.moveSpeed;
    g_lastCamMode = globalCamera.currentMode;
}