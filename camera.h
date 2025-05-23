#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "CEuler.h"
#include "CQuaternion.h"
#include "CVector.h"
#include <GL/glew.h>      // OpenGL��չ
   // GLUT���߰�

// ����֧࣬��ŷ���Ǻͱ�����Ԫ�����ֿ���ģʽ
class Camera {
public:

    // ����״̬�ṹ
    struct {
        bool isActive = false;        // �Ƿ��ڹ�����
        float progress = 0.0f;        // ��ǰ���ȣ�0.0~1.0��
        CVector startPos;             // ��ʼλ��
        CQuaternion startOrientation; // ��ʼ����
        Camera* targetCamera = nullptr; // Ŀ�����ָ��
        float duration = 1.0f;        // ������ʱ��
    } transition;

    void StartTransitionTo(Camera& target, float duration); // ��������
    bool UpdateTransition(float deltaTime); // ���¹���״̬�������Ƿ����


    // ����ģʽö�٣�ŷ����ģʽ��EULER���ͱ�����Ԫ��ģʽ��LOCAL��
    enum ControlMode { EULER, LOCAL };

    CVector position;         // ����ռ��е�λ������
    CVector origonPos;
    CQuaternion orientation; // ��Ԫ����ʾ�ĳ���
    CEuler eulerAngles;       // ŷ���Ǳ�ʾ����ת�������ƣ�
    ControlMode currentMode;  // ��ǰ����ģʽ
    float speedLen;
    float moveSpeed;          // �ƶ��ٶȵ�λ/��
    float rotateSpeed;        // ��ת�ٶȽǶ�/��
    bool online;
    CEuler allAngles;

    void RenderInfo(const char* viewType) const;
    void OptionInfo(const char* viewType) const;
    const char* GetControlModeString() const;

    Camera();
    // �л�����ģʽ����ͬ�����³�������
    void SwitchControlMode(ControlMode newMode);
    // ��ǰ�����ƶ���deltaTime ��λΪ�룩
    void MoveFront();
    // ���ҷ����ƶ���deltaTime ��λΪ�룩
    void MoveRight();
    // ���Ϸ����ƶ���deltaTime ��λΪ�룩
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveBack();
    // �ƴ�ֱ����ת��Ӱ��ƫ���ǣ�
    void RotateYaw(float angleDelta);
    // ��������ת��Ӱ�츩���ǣ�
    void RotatePitch(float angleDelta);
    // ��ǰ����ת��Ӱ���ת�ǣ�
    void RotateRoll(float angleDelta);
    // ��ȡ��ǰŷ���ǣ��Զ�ת��ģʽ��
    CEuler GetEulerAngles() const;
    // ��ȡǰ����λ����
    CVector GetForwardDir() const;
    // ��ȡ�Ϸ���λ����
    CVector GetUpDir() const;
    // ��ȡ�ҷ���λ����������������
    CVector GetRightDir() const;
    // ��ŷ���Ǹ�����Ԫ������
    void UpdateOrientationFromEuler();
    // ����Ԫ������ŷ���Ǳ�ʾ
    void UpdateEulerFromOrientation();
    // ״̬���º�ͬ�����ݲ���ӡ������Ϣ
    void Update();

    void DrawLocalAxes(float length) const;
private:
    // ������Ⱦ������������
    void RenderString(int x, int y, const char* str) const;

};

void initCamera();

#endif // CAMERA_H