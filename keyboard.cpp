// keyboard.cpp
#include "keyboard.h"
#include "ship.h"        // �����ɴ���ض���
#include "planet.h"      // ����������ض���
#include "Camera.h"
#include <GL/glut.h>
#include <iostream>

// GLUT���߰�

#define M_PI 3.14159265358979323846f
#define Fnum 2

// ����ȫ�ְ���״̬
bool keyPressed[256] = { false };
bool specialKeyPressed[256] = { false };

// �ⲿ����������������ģ���ж��壩
extern ship myShip;
extern ball* selectedPlanet;
extern bool g_wireframe;
extern Camera globalCamera, astronautCamera,shipCamera,planetCamera;
extern bool ControlingGlobal, ControllingShip, ControllingAstronaut,needGuide, ControllingShip_camera;
extern CVector deltaLight;
// ��ͨ�������»ص�

struct AstronautState {
    CVector position;
    CVector cameraPos;
    CEuler angles;
    CQuaternion orientation;
};

// ����˫֡������
AstronautState stateBuffer[Fnum]; // [0] = ��һ֡, [1] = ����һ֡
int currentStateIndex = 0;

// ���ֲ�AABBת��Ϊ����ռ�AABB
AABB transformAABB(const AABB& localBox) {
    // ��ȡ�ֲ���Χ�е�8������
    CVector vertices[8] = {
        localBox.min,
        CVector(localBox.min.x, localBox.min.y, localBox.max.z),
        CVector(localBox.min.x, localBox.max.y, localBox.min.z),
        CVector(localBox.min.x, localBox.max.y, localBox.max.z),
        CVector(localBox.max.x, localBox.min.y, localBox.min.z),
        CVector(localBox.max.x, localBox.min.y, localBox.max.z),
        CVector(localBox.max.x, localBox.max.y, localBox.min.z),
        localBox.max
    };

    // �任������ռ�
    CVector worldMin(FLT_MAX, FLT_MAX, FLT_MAX);
    CVector worldMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (int i = 0; i < 8; i++) {
        // ��ȷӦ�ñ任����M * v
        CVector worldVert = localBox.worldTransform.posMul(vertices[i]);

        worldMin.x = std::min(worldMin.x, worldVert.x);
        worldMin.y = std::min(worldMin.y, worldVert.y);
        worldMin.z = std::min(worldMin.z, worldVert.z);

        worldMax.x = std::max(worldMax.x, worldVert.x);
        worldMax.y = std::max(worldMax.y, worldVert.y);
        worldMax.z = std::max(worldMax.z, worldVert.z);
    }

    return AABB(localBox.partName, worldMin, worldMax, localBox.worldTransform);
}

// �������AABB�Ƿ���ײ
bool checkAABBCollision(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// ������ײ�㣨����AABB���ص��������ģ�
CVector calculateCollisionPoint(const AABB& shipBox, const AABB& astroBox) {
    float x = (std::max(shipBox.min.x, astroBox.min.x) + std::min(shipBox.max.x, astroBox.max.x)) * 0.5f;
    float y = (std::max(shipBox.min.y, astroBox.min.y) + std::min(shipBox.max.y, astroBox.max.y)) * 0.5f;
    float z = (std::max(shipBox.min.z, astroBox.min.z) + std::min(shipBox.max.z, astroBox.max.z)) * 0.5f;
    return CVector(x, y, z);
}

// ���ɴ����Ա����ײ
bool detectCollisions() {


    static int frameCounter = 0;
    frameCounter++;

    // ÿ5֡���һ�Σ��������Ƶ��
    //if (frameCounter % 60 != 0) return;

    bool collisionDetected = false;

    for (auto& shipLocalBox : myShip.collisionBoxes) {
        AABB shipWorldBox = transformAABB(shipLocalBox);
        if (shipLocalBox.partName == "Main Hull") continue;

        for (auto& astroLocalBox : astronaut.collisionBoxes) {
            AABB astroWorldBox = transformAABB(astroLocalBox);

            if (checkAABBCollision(shipWorldBox, astroWorldBox)) {

                CVector collisionPoint = calculateCollisionPoint(shipWorldBox, astroWorldBox);

                std::cout << "\n--- COLLISION DETECTED ---\n";
                std::cout << "Ship Part: " << shipLocalBox.partName << "\n";
                std::cout << "Astronaut Part: " << astroLocalBox.partName << "\n";
                std::cout << "Collision Point: ("
                    << collisionPoint.x << ", "
                    << collisionPoint.y << ", "
                    << collisionPoint.z << ")\n";
                std::cout << "-----------------------\n";

                collisionDetected = true;
            }
        }
    }

    if (!collisionDetected && frameCounter % 120 == 0) {
        std::cout << "No collisions detected.\n";
    }
    return collisionDetected;
}

void keyboardDown(unsigned char key, int x, int y) {
    float time = 1.5f;

    keyPressed[key] = true;
    // ���δ����Ͳ���
    switch (key) {
        case KEY_RESET:
            initShip();
            astronautCamera.origonPos = myShip.position;
            shipCamera.origonPos = myShip.position;
            break;
        case 13: // �س���
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
                // ��ǰ��ȫ���ӽǣ����ɵ��Ա�ӽ�
                globalCamera.StartTransitionTo(astronautCamera,globalCamera.transition.duration);
            }
            else {
                // ��ǰ���Ա�ӽǣ����ɵ�ȫ���ӽ�
                astronautCamera.StartTransitionTo(globalCamera,astronautCamera.transition.duration);
            }       
            break;
        case TO_GLOBAL_CAMERA: // 'i'��
            if (globalCamera.online) break;
            if (astronautCamera.online)
                astronautCamera.StartTransitionTo(globalCamera, time);
            else if (shipCamera.online)
                shipCamera.StartTransitionTo(globalCamera, time);
            break;

        case TO_ASTRONAUT_CAMERA: // 'o'��
            if (astronautCamera.online) break;
            if (globalCamera.online)
                globalCamera.StartTransitionTo(astronautCamera, time);
            else if (shipCamera.online)
                shipCamera.StartTransitionTo(astronautCamera, time);
            break;

        case TO_SHIP_CAMERA: // 'p'��
            if (shipCamera.online) {  break; }
            if (globalCamera.online)
                globalCamera.StartTransitionTo(shipCamera, time);
            else if (astronautCamera.online)
                astronautCamera.StartTransitionTo(shipCamera, time);
            break;
        case 'l': // 'l'��
            if (!globalCamera.online || myShip.targetBall == nullptr) { break; }
                globalCamera.StartTransitionTo(planetCamera, time); 
               break;
        case 'm':
            needGuide = !needGuide;
            break;
        }


}
// ��ͨ�����ͷŻص�
void keyboardUp(unsigned char key, int x, int y) {
    keyPressed[key] = false;
}
// ���ⰴ�����»ص�
void specialDown(int key, int x, int y) {
    specialKeyPressed[key] = true;
    // ���δ����������
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
// ���ⰴ���ͷŻص�
void specialUp(int key, int x, int y) {
    specialKeyPressed[key] = false;
}
// ÿ֡��ⰴ��״̬
void checkKeyStates() {
    deltaLight = CVector(0, 0, 0);
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
        if (keyPressed[CAMERA_UP]) astronautCamera.RotatePitch(2.0f);
        if (keyPressed[CAMERA_DOWN]) astronautCamera.RotatePitch(-2.0f);
        if (keyPressed[CAMERA_LEFT]) astronautCamera.RotateYaw(2.0f);
        if (keyPressed[CAMERA_RIGHT]) astronautCamera.RotateYaw(-2.0f);
        if (keyPressed[CAMERA_ROLL_LEFT]) astronautCamera.RotateRoll(-2.0f);
        if (keyPressed[CAMERA_ROLL_RIGHT]) astronautCamera.RotateRoll(2.0f);
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
    // �ٶȿ���
    if (keyPressed[SPEED_UP]) { myShip.speedLen += myShip.speedStep; }
    if (keyPressed[SPEED_DOWN]) { myShip.speedLen -= myShip.speedStep;}
    // �ٶ�����
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
            {//����ȫ�����
                if (keyPressed[CAMERA_MOVEUP])    {globalCamera.moveSpeed = 0.04f; globalCamera.MoveUp(); }
                if (keyPressed[CAMERA_MOVEDOWN])  {globalCamera.moveSpeed = 0.04f; globalCamera.MoveDown(); }
                if (keyPressed[CAMERA_MOVELEFT])  {globalCamera.moveSpeed = 0.04f; globalCamera.MoveLeft();}
                if (keyPressed[CAMERA_MOVERIGHT]) {globalCamera.moveSpeed = 0.04f; globalCamera.MoveRight();}
                if (keyPressed[CAMERA_MOVEFRONT]) {globalCamera.moveSpeed = 0.04f; globalCamera.MoveFront();
                deltaLight = globalCamera.GetForwardDir() * globalCamera.moveSpeed;
                }
                if (keyPressed[CAMERA_MOVEBACK])  {globalCamera.moveSpeed = 0.04f; globalCamera.MoveBack(); 
                deltaLight = -globalCamera.GetForwardDir() * globalCamera.moveSpeed;
                }
            }
        }
        else {
            //���Ʒɴ�
            if ((!myShip.autoPilot||myShip.targetBall == nullptr)&&(!shipCamera.online|| shipCamera.online && ControllingShip_camera)) {
                // �������
                // �޸�ԭ��ŷ���ǲ���Ϊ��Ԫ����ת
                if (keyPressed[KEY_UP]) {
                    ShipPitch(myShip.angleStep); astronautCamera.RotatePitch(myShip.angleStep);
                    shipCamera.RotatePitch(myShip.angleStep);
                }
                if (keyPressed[KEY_DOWN]) {
                    ShipPitch(-myShip.angleStep); astronautCamera.RotatePitch(-myShip.angleStep);
                    shipCamera.RotatePitch(-myShip.angleStep);
                }
                if (keyPressed[KEY_LEFT]) { ShipYaw(+myShip.angleStep);  astronautCamera.RotateYaw(myShip.angleStep);
                    shipCamera.RotateYaw(myShip.angleStep);
                }
                if (keyPressed[KEY_RIGHT]) {
                    ShipYaw(-myShip.angleStep);  astronautCamera.RotateYaw(-myShip.angleStep);
                    shipCamera.RotateYaw(-myShip.angleStep);
                }
                if (keyPressed[KEY_ROLL_LEFT]) { ShipRoll(+myShip.angleStep); astronautCamera.RotateRoll(-myShip.angleStep);
                    shipCamera.RotateRoll(-myShip.angleStep);
                }
                if (keyPressed[KEY_ROLL_RIGHT]) {
                    ShipRoll(-myShip.angleStep); astronautCamera.RotateRoll(myShip.angleStep);
                    shipCamera.RotateRoll(myShip.angleStep);
                }

                astronautCamera.Update();
                shipCamera.Update();
            }
        }       
    }
    else {
        if (ControllingAstronaut) {
            AstronautState initialState = {
                astronaut.position,
                astronautCamera.deltaPos,
                astronaut.allAngle,
                astronautCamera.orientation
            };

            // ���浽������
            stateBuffer[currentStateIndex] = initialState;

            //�Ա
            if (keyPressed[KEY_ROLL_LEFT]) { 
                astronaut.position = astronaut.position + astronaut.direction * astronaut.speedLen;
                //astronautCamera.deltaPos = astronautCamera.deltaPos + astronaut.finalDir * astronaut.speedLen;
                astronautCamera.deltaPos = astronautCamera.deltaPos + astronaut.finalDir * astronaut.speedLen;
            }
            if (keyPressed[KEY_ROLL_RIGHT]) { 
                astronaut.position = astronaut.position - astronaut.direction * astronaut.speedLen; 
                astronautCamera.deltaPos = astronautCamera.deltaPos - astronaut.finalDir * astronaut.speedLen;
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

            if (detectCollisions()) {
                // ���˵���ʼ״̬�����봦��ǰ��״̬��
                AstronautState initialState2 = stateBuffer[(currentStateIndex + 1) % Fnum];
                astronaut.position = initialState2.position;
                astronautCamera.deltaPos = initialState2.cameraPos;
                astronaut.allAngle = initialState2.angles;
                astronautCamera.orientation = initialState2.orientation;
                astronautCamera.UpdateEulerFromOrientation();

            }
            // ����������ȷ�����ᳬ����Χ��
            currentStateIndex = (currentStateIndex + 1) % Fnum;


            if (keyPressed[KEY_ROLL_LEFT] || keyPressed[KEY_ROLL_RIGHT]) {
                // ʹ��ʱ�����ڶ��Ƕȣ��γɶ���
                float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f; // ��ȡ��ǰʱ�䣨�룩
                float swing = sin(time * 8.0f) * 45.0f; // ����Ƶ��(8)�ͷ���(30��)
                astronaut.rightLegAngle = swing;
                astronaut.leftLegAngle = -swing; // �����Ƚ���ڶ�
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
