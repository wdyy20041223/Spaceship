#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GL/glew.h>      // OpenGL扩展
#include <GL/glut.h>      // GLUT工具包
#include "Astronaut.h"

#define SPEED_UP    'u'//飞船加减速
#define SPEED_DOWN  'j'

#define KEY_UP      'w'//飞船或太空人方向控制
#define KEY_DOWN    's'
#define KEY_LEFT    'a'
#define KEY_RIGHT   'd'
#define KEY_ROLL_LEFT  'q'  // 新增滚转控制
#define KEY_ROLL_RIGHT 'e'

#define MAN_FRONT      'q'//太空人前进和后退
#define MAN_BACK    'e'

#define CAMERA_UP      't'//全局相机或太空人相机旋转方向控制
#define CAMERA_DOWN    'g'
#define CAMERA_LEFT    'f'
#define CAMERA_RIGHT   'h'
#define CAMERA_ROLL_LEFT  'r'  // 新增滚转控制
#define CAMERA_ROLL_RIGHT 'y'

#define KEY_RESET   ' '//飞船复位

#define MODE_SWITCH  'c'//切换控制模式
#define MODE_SWITCH_CAMERA  'v'//切换控制模式
#define TO_GLOBAL_CAMERA 'i'//切换相机
#define TO_ASTRONAUT_CAMERA 'o'
#define TO_SHIP_CAMERA 'p'

//相机平移
#define CAMERA_MOVEUP      'w'    // 相机上移
#define CAMERA_MOVEDOWN   's'    // 相机下移
#define CAMERA_MOVELEFT   'a'    // 相机左移
#define CAMERA_MOVERIGHT   'd'    // 相机右移
#define CAMERA_MOVEFRONT   'q'    // 相机后移
#define CAMERA_MOVEBACK    'e'    // 相机前移



// 声明全局按键状态数组
extern bool keyPressed[256];
extern bool specialKeyPressed[256];
extern Astronaut astronaut;

// 声明键盘回调函数
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);

// 声明每帧状态检测函数
void checkKeyStates();



#endif // KEYBOARD_H