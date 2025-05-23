// keyboard.cpp
#include "keyboard.h"
#include "ship.h"        // 包含飞船相关定义
#include "planet.h"      // 包含星球相关定义
#include "Camera.h"
#include <GL/glut.h>

// GLUT工具包

#define M_PI 3.14159265358979323846f

// 定义全局按键状态
bool keyPressed[256] = { false };
bool specialKeyPressed[256] = { false };

// 外部变量声明（在其它模块中定义）
extern ship myShip;
extern ball* selectedPlanet;
extern bool g_wireframe;
extern Camera globalCamera, astronautCamera,shipCamera,planetCamera;
extern bool ControlingGlobal, ControllingShip, ControllingAstronaut,needGuide, ControllingShip_camera;

// 普通按键按下回调
void keyboardDown(unsigned char key, int x, int y) {
    float time = 1.5f;

    keyPressed[key] = true;
    // 单次触发型操作
    switch (key) {
        case KEY_RESET:
            initShip();
            astronautCamera.origonPos = myShip.position;
            shipCamera.origonPos = myShip.position;
            break;
        case 13: // 回车键
            if (selectedPlanet) {
                myShip.autoPilot = !myShip.autoPilot;
                if (myShip.autoPilot)
                    myShip.targetBall = selectedPlanet;
                else {
                    if (myShip.speedLen <= myShip.speedStep * 2) {
                        myShip.speedLen = myShip.speedTempLen;
                    }
                }
            }
            break;
        case MODE_SWITCH:
            if (globalCamera.online) {
                if (globalCamera.currentMode == Camera::EULER) {
                    globalCamera.SwitchControlMode(Camera::LOCAL);
                }
                else {
                    globalCamera.SwitchControlMode(Camera::EULER);
                }
                break;
            }
            else if(astronautCamera.online) {
                if (astronautCamera.currentMode == Camera::EULER) {
                    astronautCamera.SwitchControlMode(Camera::LOCAL);
                }
                else {
                    astronautCamera.SwitchControlMode(Camera::EULER);
                }
                break;
            }
            else {
                if (shipCamera.currentMode == Camera::EULER) {
                    shipCamera.SwitchControlMode(Camera::LOCAL);
                }
                else {
                    shipCamera.SwitchControlMode(Camera::EULER);
                }
                break;
            }
        
        case MODE_SWITCH_CAMERA:
            if (globalCamera.online) {
                // 当前是全局视角，过渡到宇航员视角
                globalCamera.StartTransitionTo(astronautCamera,globalCamera.transition.duration);
            }
            else {
                // 当前是宇航员视角，过渡到全局视角
                astronautCamera.StartTransitionTo(globalCamera,astronautCamera.transition.duration);
            }       
            break;
        case TO_GLOBAL_CAMERA: // 'i'键
            if (globalCamera.online) break;
            if (astronautCamera.online)
                astronautCamera.StartTransitionTo(globalCamera, time);
            else if (shipCamera.online)
                shipCamera.StartTransitionTo(globalCamera, time);
            break;

        case TO_ASTRONAUT_CAMERA: // 'o'键
            if (astronautCamera.online) break;
            if (globalCamera.online)
                globalCamera.StartTransitionTo(astronautCamera, time);
            else if (shipCamera.online)
                shipCamera.StartTransitionTo(astronautCamera, time);
            break;

        case TO_SHIP_CAMERA: // 'p'键
            if (shipCamera.online) {  break; }
            if (globalCamera.online)
                globalCamera.StartTransitionTo(shipCamera, time);
            else if (astronautCamera.online)
                astronautCamera.StartTransitionTo(shipCamera, time);
            break;
        case 'l': // 'l'键
            if (!globalCamera.online || myShip.targetBall == nullptr) { break; }
                globalCamera.StartTransitionTo(planetCamera, time); 
               break;
        case 'm':
            needGuide = !needGuide;
            break;
        }


}
// 普通按键释放回调
void keyboardUp(unsigned char key, int x, int y) {
    keyPressed[key] = false;
}
// 特殊按键按下回调
void specialDown(int key, int x, int y) {
    specialKeyPressed[key] = true;
    // 单次触发型特殊键
    switch (key) {
    case GLUT_KEY_F1:
        g_wireframe = !g_wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, g_wireframe ? GL_LINE : GL_FILL);
        glutPostRedisplay();
        break;
    case GLUT_KEY_F2:  
        if (globalCamera.online)
            ControllingShip = !ControllingShip;
        else if (astronautCamera.online)
            ControllingAstronaut = !ControllingAstronaut;
        else if (shipCamera.online)
            ControllingShip_camera = !ControllingShip_camera;
        break;
    
    }
    
        

}
// 特殊按键释放回调
void specialUp(int key, int x, int y) {
    specialKeyPressed[key] = false;
}
// 每帧检测按键状态
void checkKeyStates() {
    if (globalCamera.transition.isActive || astronautCamera.transition.isActive || shipCamera.transition.isActive) {
        return;
    }
    globalCamera.moveSpeed = 0;

    if (globalCamera.online) {
        if (keyPressed[CAMERA_UP]) globalCamera.RotatePitch(1.0f);
        if (keyPressed[CAMERA_DOWN]) globalCamera.RotatePitch(-1.0f);
        if (keyPressed[CAMERA_LEFT]) globalCamera.RotateYaw(1.0f);
        if (keyPressed[CAMERA_RIGHT]) globalCamera.RotateYaw(-1.0f);
        if (keyPressed[CAMERA_ROLL_LEFT]) globalCamera.RotateRoll(-1.0f);
        if (keyPressed[CAMERA_ROLL_RIGHT]) globalCamera.RotateRoll(1.0f);

    }
    else if(astronautCamera.online){
        if (keyPressed[CAMERA_UP]) astronautCamera.RotatePitch(1.0f);
        if (keyPressed[CAMERA_DOWN]) astronautCamera.RotatePitch(-1.0f);
        if (keyPressed[CAMERA_LEFT]) astronautCamera.RotateYaw(1.0f);
        if (keyPressed[CAMERA_RIGHT]) astronautCamera.RotateYaw(-1.0f);
        if (keyPressed[CAMERA_ROLL_LEFT]) astronautCamera.RotateRoll(-1.0f);
        if (keyPressed[CAMERA_ROLL_RIGHT]) astronautCamera.RotateRoll(1.0f);
    }
    else {
        float delta = 0.5f;
        if (keyPressed[CAMERA_UP]) shipCamera.RotatePitch(delta);
        if (keyPressed[CAMERA_DOWN]) shipCamera.RotatePitch(-delta);
        if (keyPressed[CAMERA_LEFT]) shipCamera.RotateYaw(delta);
        if (keyPressed[CAMERA_RIGHT]) shipCamera.RotateYaw(-delta);
        if (keyPressed[CAMERA_ROLL_LEFT]) shipCamera.RotateRoll(-delta);
        if (keyPressed[CAMERA_ROLL_RIGHT]) shipCamera.RotateRoll(delta);
    }
    // 速度控制
    if (keyPressed[SPEED_UP]) { myShip.speedLen += myShip.speedStep; }
    if (keyPressed[SPEED_DOWN]) { myShip.speedLen -= myShip.speedStep;}
    // 速度限制
    if (myShip.speedLen < 0) {
        myShip.speedLen = 0;
    }

    if (shipCamera.online) {
        if (!ControllingShip_camera) {
            if (keyPressed[CAMERA_MOVEUP]) { shipCamera.MoveUp(); }
            if (keyPressed[CAMERA_MOVEDOWN]) { shipCamera.MoveDown(); }
            if (keyPressed[CAMERA_MOVELEFT]) { shipCamera.MoveLeft(); }
            if (keyPressed[CAMERA_MOVERIGHT]) { shipCamera.MoveRight(); }
            if (keyPressed[CAMERA_MOVEFRONT]) { shipCamera.MoveFront(); }
            if (keyPressed[CAMERA_MOVEBACK]) { shipCamera.MoveBack(); }
        }
    }

    if (ControlingGlobal || shipCamera.online) {
        if (!ControllingShip && !shipCamera.online) {
            {//控制全局相机
                if (keyPressed[CAMERA_MOVEUP])    {globalCamera.moveSpeed = 0.04f; globalCamera.MoveUp(); }
                if (keyPressed[CAMERA_MOVEDOWN])  {globalCamera.moveSpeed = 0.04f; globalCamera.MoveDown(); }
                if (keyPressed[CAMERA_MOVELEFT])  {globalCamera.moveSpeed = 0.04f; globalCamera.MoveLeft();}
                if (keyPressed[CAMERA_MOVERIGHT]) {globalCamera.moveSpeed = 0.04f; globalCamera.MoveRight();}
                if (keyPressed[CAMERA_MOVEFRONT]) {globalCamera.moveSpeed = 0.04f; globalCamera.MoveFront();}
                if (keyPressed[CAMERA_MOVEBACK])  {globalCamera.moveSpeed = 0.04f; globalCamera.MoveBack(); }      
            }
        }
        else {
            //控制飞船
            if ((!myShip.autoPilot||myShip.targetBall == nullptr)&&(!shipCamera.online|| shipCamera.online && ControllingShip_camera)) {
                // 方向控制
                if (keyPressed[KEY_UP]) {
                    myShip.upDownAngle += myShip.angleStep; 
                    astronautCamera.RotatePitch(myShip.angleStep);
                    astronautCamera.Update();
                    shipCamera.RotatePitch(myShip.angleStep);
                    shipCamera.Update();
                }
                if (keyPressed[KEY_DOWN]) {
                    myShip.upDownAngle -= myShip.angleStep; 
                    astronautCamera.RotatePitch(-myShip.angleStep);
                    astronautCamera.Update();
                    shipCamera.RotatePitch(-myShip.angleStep);
                    shipCamera.Update();
                }
                if (keyPressed[KEY_LEFT]) {
                    myShip.leftRightAngle += myShip.angleStep; 
                    astronautCamera.RotateYaw(myShip.angleStep);
                    astronautCamera.Update();
                    shipCamera.RotateYaw(myShip.angleStep);
                    shipCamera.Update();
                }
                if (keyPressed[KEY_RIGHT]) {
                    myShip.leftRightAngle -= myShip.angleStep;
                    astronautCamera.RotateYaw(-myShip.angleStep);
                    astronautCamera.Update();
                    shipCamera.RotateYaw(-myShip.angleStep);
                    shipCamera.Update();
                }
                if (keyPressed[KEY_ROLL_LEFT]) {
                    myShip.rollAngle -= myShip.angleStep;
                    astronautCamera.RotateRoll(-myShip.angleStep); // 改为正角度
                    astronautCamera.Update(); // 确保更新状态
                    shipCamera.RotateRoll(-myShip.angleStep); // 改为正角度
                    shipCamera.Update(); // 确保更新状态
                }
                if (keyPressed[KEY_ROLL_RIGHT]) {
                    myShip.rollAngle += myShip.angleStep;
                    astronautCamera.RotateRoll(+myShip.angleStep); // 改为负角度
                    astronautCamera.Update(); // 确保更新状态
                    shipCamera.RotateRoll(+myShip.angleStep); // 改为负角度
                    shipCamera.Update(); // 确保更新状态
                }
            }
        }       
    }
    else {
        if (ControllingAstronaut) {
            //宇航员
            if (keyPressed[KEY_ROLL_LEFT]) { astronaut.position = astronaut.position + astronaut.direction * astronaut.speedLen;
            astronautCamera.origonPos = astronautCamera.origonPos + astronaut.direction * astronaut.speedLen;
            }
            if (keyPressed[KEY_ROLL_RIGHT]) { astronaut.position = astronaut.position - astronaut.direction * astronaut.speedLen; 
            astronautCamera.origonPos = astronautCamera.origonPos - astronaut.direction * astronaut.speedLen;
            }

            if (keyPressed[KEY_LEFT]) {
                astronaut.allAngle.h = astronaut.allAngle.h + astronaut.angleStep;
                astronautCamera.RotateYaw(astronaut.angleStep);
            }
            if (keyPressed[KEY_RIGHT]) {
                astronaut.allAngle.h = astronaut.allAngle.h - astronaut.angleStep;
                astronautCamera.RotateYaw(-astronaut.angleStep);
            }
            if (keyPressed[KEY_UP]) {
                astronaut.allAngle.p = astronaut.allAngle.p + astronaut.angleStep;
                astronautCamera.RotatePitch(astronaut.angleStep);
            }
            if (keyPressed[KEY_DOWN]) {
                astronaut.allAngle.p = astronaut.allAngle.p - astronaut.angleStep;
                astronautCamera.RotatePitch(-astronaut.angleStep);
            }

            if (keyPressed[KEY_ROLL_LEFT] || keyPressed[KEY_ROLL_RIGHT]) {
                // 使用时间计算摆动角度，形成动画
                float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f; // 获取当前时间（秒）
                float swing = sin(time * 8.0f) * 45.0f; // 调整频率(8)和幅度(30度)
                astronaut.rightLegAngle = swing;
                astronaut.leftLegAngle = -swing; // 左右腿交替摆动
                astronautCamera.Update();
            }
            else {
                astronaut.rightLegAngle = 0.0f;
                astronaut.leftLegAngle = 0.0f;
            }
        }
        else {
            if (keyPressed[CAMERA_MOVEUP]) { astronautCamera.MoveUp(); }
            if (keyPressed[CAMERA_MOVEDOWN]) {  astronautCamera.MoveDown(); }
            if (keyPressed[CAMERA_MOVELEFT]) {  astronautCamera.MoveLeft(); }
            if (keyPressed[CAMERA_MOVERIGHT]) { astronautCamera.MoveRight(); }
            if (keyPressed[CAMERA_MOVEFRONT]) {  astronautCamera.MoveFront(); }
            if (keyPressed[CAMERA_MOVEBACK]) {  astronautCamera.MoveBack(); }
        }
    }   
}
