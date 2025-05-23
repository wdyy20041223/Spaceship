#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "CEuler.h"
#include "CQuaternion.h"
#include "CVector.h"
#include <GL/glew.h>      // OpenGL扩展
   // GLUT工具包

// 相机类，支持欧拉角和本地四元数两种控制模式
class Camera {
public:

    // 过渡状态结构
    struct {
        bool isActive = false;        // 是否处于过渡中
        float progress = 0.0f;        // 当前进度（0.0~1.0）
        CVector startPos;             // 起始位置
        CQuaternion startOrientation; // 起始朝向
        Camera* targetCamera = nullptr; // 目标相机指针
        float duration = 1.0f;        // 过渡总时间
    } transition;

    void StartTransitionTo(Camera& target, float duration); // 启动过渡
    bool UpdateTransition(float deltaTime); // 更新过渡状态，返回是否完成


    // 控制模式枚举：欧拉角模式（EULER）和本地四元数模式（LOCAL）
    enum ControlMode { EULER, LOCAL };

    CVector position;         // 世界空间中的位置坐标
    CVector origonPos;
    CQuaternion orientation; // 四元数表示的朝向
    CEuler eulerAngles;       // 欧拉角表示的旋转（弧度制）
    ControlMode currentMode;  // 当前控制模式
    float speedLen;
    float moveSpeed;          // 移动速度单位/秒
    float rotateSpeed;        // 旋转速度角度/秒
    bool online;
    CEuler allAngles;

    void RenderInfo(const char* viewType) const;
    void OptionInfo(const char* viewType) const;
    const char* GetControlModeString() const;

    Camera();
    // 切换控制模式，并同步更新朝向数据
    void SwitchControlMode(ControlMode newMode);
    // 沿前方向移动（deltaTime 单位为秒）
    void MoveFront();
    // 沿右方向移动（deltaTime 单位为秒）
    void MoveRight();
    // 沿上方向移动（deltaTime 单位为秒）
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveBack();
    // 绕垂直轴旋转（影响偏航角）
    void RotateYaw(float angleDelta);
    // 绕右轴旋转（影响俯仰角）
    void RotatePitch(float angleDelta);
    // 绕前轴旋转（影响滚转角）
    void RotateRoll(float angleDelta);
    // 获取当前欧拉角（自动转换模式）
    CEuler GetEulerAngles() const;
    // 获取前方向单位向量
    CVector GetForwardDir() const;
    // 获取上方向单位向量
    CVector GetUpDir() const;
    // 获取右方向单位向量（辅助函数）
    CVector GetRightDir() const;
    // 从欧拉角更新四元数朝向
    void UpdateOrientationFromEuler();
    // 从四元数更新欧拉角表示
    void UpdateEulerFromOrientation();
    // 状态更新后同步数据并打印调试信息
    void Update();

    void DrawLocalAxes(float length) const;
private:
    // 文字渲染辅助方法声明
    void RenderString(int x, int y, const char* str) const;

};

void initCamera();

#endif // CAMERA_H