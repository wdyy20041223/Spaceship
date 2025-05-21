#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GL/glew.h>      // OpenGL��չ
#include <GL/glut.h>      // GLUT���߰�
#include "Astronaut.h"

#define SPEED_UP    'u'//�ɴ��Ӽ���
#define SPEED_DOWN  'j'

#define KEY_UP      'w'//�ɴ���̫���˷������
#define KEY_DOWN    's'
#define KEY_LEFT    'a'
#define KEY_RIGHT   'd'
#define KEY_ROLL_LEFT  'q'  // ������ת����
#define KEY_ROLL_RIGHT 'e'

#define MAN_FRONT      'q'//̫����ǰ���ͺ���
#define MAN_BACK    'e'

#define CAMERA_UP      't'//ȫ�������̫���������ת�������
#define CAMERA_DOWN    'g'
#define CAMERA_LEFT    'f'
#define CAMERA_RIGHT   'h'
#define CAMERA_ROLL_LEFT  'r'  // ������ת����
#define CAMERA_ROLL_RIGHT 'y'

#define KEY_RESET   ' '//�ɴ���λ

#define MODE_SWITCH  'c'//�л�����ģʽ
#define MODE_SWITCH_CAMERA  'v'//�л�����ģʽ
#define TO_GLOBAL_CAMERA 'i'//�л����
#define TO_ASTRONAUT_CAMERA 'o'
#define TO_SHIP_CAMERA 'p'

//���ƽ��
#define CAMERA_MOVEUP      'w'    // �������
#define CAMERA_MOVEDOWN   's'    // �������
#define CAMERA_MOVELEFT   'a'    // �������
#define CAMERA_MOVERIGHT   'd'    // �������
#define CAMERA_MOVEFRONT   'q'    // �������
#define CAMERA_MOVEBACK    'e'    // ���ǰ��



// ����ȫ�ְ���״̬����
extern bool keyPressed[256];
extern bool specialKeyPressed[256];
extern Astronaut astronaut;

// �������̻ص�����
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);

// ����ÿ֡״̬��⺯��
void checkKeyStates();



#endif // KEYBOARD_H