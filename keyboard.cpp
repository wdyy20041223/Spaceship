// keyboard.cpp
#include "keyboard.h"
#include "ship.h"        // �����ɴ���ض���
#include "planet.h"      // ����������ض���
#include "Camera.h"
#include <GL/glut.h>
#include <iostream>
#include "global.h"

// GLUT���߰�

#define M_PI 3.14159265358979323846f
#define Fnum 2

std::vector<std::pair<AABB, char>> g_debugAABBs;

// ����ȫ�ְ���״̬
bool keyPressed[256] = { false };
bool specialKeyPressed[256] = { false };

cinfo cInfo[2];
static int cInfoIndex = 0;

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

    CVector worldVert[8];

    for (int i = 0; i < 8; i++) {
        // ��ȷӦ�ñ任����M * v
        worldVert[i] = localBox.worldTransform.posMul(vertices[i]);
    }

    AABB temp = AABB(localBox.partName, worldMin, worldMax, localBox.worldTransform);
    // ���ƶ�������
    for (int i = 0; i < 8; i++) {
        temp.pos[i] = worldVert[i];
    }

    return temp;
}

// ��ȫ��һ����������
CVector safeNormalize(const CVector& v, const CVector& fallback) {
    float length = v.len();
    if (length < 1e-6f) {
        // ������������ǳ��̵�������ʹ�ñ�������
        return fallback.Normalized();
    }
    return v * (1.0f / length);
}

// ��ȫ�����������ض����ϵ�ͶӰ����
float safeLength(const CVector& v, const CVector& axis) {
    // ����ͶӰ����
    float projection = v.dotMul(axis);

    // ���ڼ��̵��ᣬ����������ԭʼ����
    if (axis.len() < 1e-6f) return v.len();

    return std::abs(projection);
}

// ����׳��OBB��ײ��⺯��
bool CheckOBBCollision(const AABB& a, const AABB& b) {
    // 1. ��ȡOBB��8������
    const CVector* aVertices = a.pos;
    const CVector* bVertices = b.pos;

    // 2. ����OBB�����ĵ�
    CVector aCenter(0, 0, 0);
    CVector bCenter(0, 0, 0);

    for (int i = 0; i < 8; ++i) {
        aCenter = aCenter + aVertices[i];
        bCenter = bCenter + bVertices[i];
    }
    aCenter = aCenter * 0.125f; // /= 8
    bCenter = bCenter * 0.125f;

    // 3. ��������������Χ�����ĵ�����
    CVector v = bCenter - aCenter;

    // 4. ��ȫ�����᷽��
    CVector aAxis[3], bAxis[3];
    float aExtents[3], bExtents[3];

    // ��ȫ����A����Ͱ볤
    aAxis[0] = safeNormalize(aVertices[1] - aVertices[0], CVector(1, 0, 0));
    aAxis[1] = safeNormalize(aVertices[3] - aVertices[0], CVector(0, 1, 0));
    aAxis[2] = safeNormalize(aVertices[4] - aVertices[0], CVector(0, 0, 1));

    aExtents[0] = safeLength(aVertices[0] - aCenter, aAxis[0]);
    aExtents[1] = safeLength(aVertices[3] - aCenter, aAxis[1]);
    aExtents[2] = safeLength(aVertices[4] - aCenter, aAxis[2]);

    // ��ȫ����B����Ͱ볤
    bAxis[0] = safeNormalize(bVertices[1] - bVertices[0], CVector(1, 0, 0));
    bAxis[1] = safeNormalize(bVertices[3] - bVertices[0], CVector(0, 1, 0));
    bAxis[2] = safeNormalize(bVertices[4] - bVertices[0], CVector(0, 0, 1));

    bExtents[0] = safeLength(bVertices[0] - bCenter, bAxis[0]);
    bExtents[1] = safeLength(bVertices[3] - bCenter, bAxis[1]);
    bExtents[2] = safeLength(bVertices[4] - bCenter, bAxis[2]);

    // 5. ��ʼ��ͶӰ����
    float R[3][3];   // ��ת����
    float FR[3][3];  // ���ݲ�ľ�����ת����
    for (int i = 0; i < 3; ++i) {
        for (int k = 0; k < 3; ++k) {
            R[i][k] = aAxis[i].dotMul(bAxis[k]);
            FR[i][k] = std::abs(R[i][k]) + 1e-6f;
        }
    }

    // 6. ���������
    // ����A����
    for (int i = 0; i < 3; ++i) {
        float ra = aExtents[i];
        float rb = bExtents[0] * FR[i][0] + bExtents[1] * FR[i][1] + bExtents[2] * FR[i][2];
        float t = std::abs(v.dotMul(aAxis[i]));
        if (t > ra + rb) return false;
    }

    // ����B����
    for (int k = 0; k < 3; ++k) {
        float ra = aExtents[0] * FR[0][k] + aExtents[1] * FR[1][k] + aExtents[2] * FR[2][k];
        float rb = bExtents[k];
        float t = std::abs(
            v.dotMul(bAxis[0]) * R[0][k] +
            v.dotMul(bAxis[1]) * R[1][k] +
            v.dotMul(bAxis[2]) * R[2][k]
        );
        if (t > ra + rb) return false;
    }

    // 7. ��ȫ���Բ����
    const float minAxisLength = 1e-6f; // ��С�ɽ����᳤��

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            CVector cross = aAxis[i].crossMul(bAxis[j]);
            float crossLength = cross.len();

            // ����̫�̵Ĳ����
            if (crossLength < minAxisLength) continue;

            CVector L = cross * (1.0f / crossLength); // ��ȫ��һ��

            float ra = 0;
            for (int k = 0; k < 3; ++k) {
                ra += aExtents[k] * std::abs(aAxis[k].dotMul(L));
            }

            float rb = 0;
            for (int k = 0; k < 3; ++k) {
                rb += bExtents[k] * std::abs(bAxis[k].dotMul(L));
            }

            float t = std::abs(v.dotMul(L));
            if (t > ra + rb) return false;
        }
    }

    // 8. ���з��������ͨ����������ײ
    return true;
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
    static int tempC = 0;
    g_debugAABBs.clear();  // �����һ֡�ĵ�����Ϣ

    bool collisionDetected = false;

    for (auto& shipLocalBox : myShip.collisionBoxes) {
        if (shipLocalBox.partName == "Main Hull") continue;
        AABB shipWorldBox = transformAABB(shipLocalBox);       
        g_debugAABBs.push_back({ shipWorldBox , 's' });

        for (auto& astroLocalBox : astronaut.collisionBoxes) {
            AABB astroWorldBox = transformAABB(astroLocalBox);
            g_debugAABBs.push_back({ astroWorldBox, 'a' });

            if (CheckOBBCollision(shipWorldBox, astroWorldBox)) {
               
                CVector collisionPoint = calculateCollisionPoint(shipWorldBox, astroWorldBox);
                cInfo[cInfoIndex].astroPart = astroWorldBox.partName;
                cInfo[cInfoIndex].shipPart = shipWorldBox.partName;
                cInfo[cInfoIndex].collisionPoint = collisionPoint;
                cInfoIndex = (cInfoIndex + 1) % 2;

                collisionDetected = true;
            }
        }
    }

    if (!collisionDetected) {
        tempC++;
        if (tempC == 90) {
            for (int i = 0; i < 2; i++) {
                cInfo[i].shipPart = "NULL";
                cInfo[i].astroPart = "NULL";
                cInfo[i].collisionPoint = CVector(0, 0, 0);
            }
            tempC = 0;
        }
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
