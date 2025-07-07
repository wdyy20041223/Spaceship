// keyboard.cpp
#include "keyboard.h"
#include "ship.h"        // 包含飞船相关定义
#include "planet.h"      // 包含星球相关定义
#include "Camera.h"
#include <GL/glut.h>
#include <iostream>
#include "global.h"

// GLUT工具包

#define M_PI 3.14159265358979323846f
#define Fnum 2

std::vector<std::pair<AABB, char>> g_debugAABBs;

// 定义全局按键状态
bool keyPressed[256] = { false };
bool specialKeyPressed[256] = { false };

cinfo cInfo[2];
static int cInfoIndex = 0;

// 外部变量声明（在其它模块中定义）
extern ship myShip;
extern ball* selectedPlanet;
extern bool g_wireframe;
extern Camera globalCamera, astronautCamera,shipCamera,planetCamera;
extern bool ControlingGlobal, ControllingShip, ControllingAstronaut,needGuide, ControllingShip_camera;
extern CVector deltaLight;
// 普通按键按下回调

struct AstronautState {
    CVector position;
    CVector cameraPos;
    CEuler angles;
    CQuaternion orientation;
};

// 创建双帧缓冲区
AstronautState stateBuffer[Fnum]; // [0] = 上一帧, [1] = 上上一帧
int currentStateIndex = 0;

// 将局部AABB转换为世界空间AABB
AABB transformAABB(const AABB& localBox) {
    // 获取局部包围盒的8个顶点
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

    // 变换到世界空间
    CVector worldMin(FLT_MAX, FLT_MAX, FLT_MAX);
    CVector worldMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    CVector worldVert[8];

    for (int i = 0; i < 8; i++) {
        // 正确应用变换矩阵：M * v
        worldVert[i] = localBox.worldTransform.posMul(vertices[i]);
    }

    AABB temp = AABB(localBox.partName, worldMin, worldMax, localBox.worldTransform);
    // 复制顶点数据
    for (int i = 0; i < 8; i++) {
        temp.pos[i] = worldVert[i];
    }

    return temp;
}

// 安全归一化向量函数
CVector safeNormalize(const CVector& v, const CVector& fallback) {
    float length = v.len();
    if (length < 1e-6f) {
        // 对于零向量或非常短的向量，使用备用向量
        return fallback.Normalized();
    }
    return v * (1.0f / length);
}

// 安全计算向量在特定轴上的投影长度
float safeLength(const CVector& v, const CVector& axis) {
    // 计算投影长度
    float projection = v.dotMul(axis);

    // 对于极短的轴，返回向量的原始长度
    if (axis.len() < 1e-6f) return v.len();

    return std::abs(projection);
}

// 更健壮的OBB碰撞检测函数
bool CheckOBBCollision(const AABB& a, const AABB& b) {
    // 1. 获取OBB的8个顶点
    const CVector* aVertices = a.pos;
    const CVector* bVertices = b.pos;

    // 2. 计算OBB的中心点
    CVector aCenter(0, 0, 0);
    CVector bCenter(0, 0, 0);

    for (int i = 0; i < 8; ++i) {
        aCenter = aCenter + aVertices[i];
        bCenter = bCenter + bVertices[i];
    }
    aCenter = aCenter * 0.125f; // /= 8
    bCenter = bCenter * 0.125f;

    // 3. 计算连接两个包围盒中心的向量
    CVector v = bCenter - aCenter;

    // 4. 安全计算轴方向
    CVector aAxis[3], bAxis[3];
    float aExtents[3], bExtents[3];

    // 安全计算A的轴和半长
    aAxis[0] = safeNormalize(aVertices[1] - aVertices[0], CVector(1, 0, 0));
    aAxis[1] = safeNormalize(aVertices[3] - aVertices[0], CVector(0, 1, 0));
    aAxis[2] = safeNormalize(aVertices[4] - aVertices[0], CVector(0, 0, 1));

    aExtents[0] = safeLength(aVertices[0] - aCenter, aAxis[0]);
    aExtents[1] = safeLength(aVertices[3] - aCenter, aAxis[1]);
    aExtents[2] = safeLength(aVertices[4] - aCenter, aAxis[2]);

    // 安全计算B的轴和半长
    bAxis[0] = safeNormalize(bVertices[1] - bVertices[0], CVector(1, 0, 0));
    bAxis[1] = safeNormalize(bVertices[3] - bVertices[0], CVector(0, 1, 0));
    bAxis[2] = safeNormalize(bVertices[4] - bVertices[0], CVector(0, 0, 1));

    bExtents[0] = safeLength(bVertices[0] - bCenter, bAxis[0]);
    bExtents[1] = safeLength(bVertices[3] - bCenter, bAxis[1]);
    bExtents[2] = safeLength(bVertices[4] - bCenter, bAxis[2]);

    // 5. 初始化投影矩阵
    float R[3][3];   // 旋转矩阵
    float FR[3][3];  // 带容差的绝对旋转矩阵
    for (int i = 0; i < 3; ++i) {
        for (int k = 0; k < 3; ++k) {
            R[i][k] = aAxis[i].dotMul(bAxis[k]);
            FR[i][k] = std::abs(R[i][k]) + 1e-6f;
        }
    }

    // 6. 分离轴测试
    // 测试A的轴
    for (int i = 0; i < 3; ++i) {
        float ra = aExtents[i];
        float rb = bExtents[0] * FR[i][0] + bExtents[1] * FR[i][1] + bExtents[2] * FR[i][2];
        float t = std::abs(v.dotMul(aAxis[i]));
        if (t > ra + rb) return false;
    }

    // 测试B的轴
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

    // 7. 安全测试叉积轴
    const float minAxisLength = 1e-6f; // 最小可接受轴长度

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            CVector cross = aAxis[i].crossMul(bAxis[j]);
            float crossLength = cross.len();

            // 跳过太短的叉积轴
            if (crossLength < minAxisLength) continue;

            CVector L = cross * (1.0f / crossLength); // 安全归一化

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

    // 8. 所有分离轴测试通过，存在碰撞
    return true;
}



// 计算碰撞点（两个AABB的重叠区域中心）
CVector calculateCollisionPoint(const AABB& shipBox, const AABB& astroBox) {
    float x = (std::max(shipBox.min.x, astroBox.min.x) + std::min(shipBox.max.x, astroBox.max.x)) * 0.5f;
    float y = (std::max(shipBox.min.y, astroBox.min.y) + std::min(shipBox.max.y, astroBox.max.y)) * 0.5f;
    float z = (std::max(shipBox.min.z, astroBox.min.z) + std::min(shipBox.max.z, astroBox.max.z)) * 0.5f;
    return CVector(x, y, z);
}

// 检测飞船和宇航员的碰撞
bool detectCollisions() {
    static int tempC = 0;
    g_debugAABBs.clear();  // 清除上一帧的调试信息

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
                if (keyPressed[CAMERA_MOVEFRONT]) {globalCamera.moveSpeed = 0.04f; globalCamera.MoveFront();
                deltaLight = globalCamera.GetForwardDir() * globalCamera.moveSpeed;
                }
                if (keyPressed[CAMERA_MOVEBACK])  {globalCamera.moveSpeed = 0.04f; globalCamera.MoveBack(); 
                deltaLight = -globalCamera.GetForwardDir() * globalCamera.moveSpeed;
                }
            }
        }
        else {
            //控制飞船
            if ((!myShip.autoPilot||myShip.targetBall == nullptr)&&(!shipCamera.online|| shipCamera.online && ControllingShip_camera)) {
                // 方向控制
                // 修改原有欧拉角操作为四元数旋转
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

            // 保存到缓冲区
            stateBuffer[currentStateIndex] = initialState;

            //宇航员
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
                // 回退到初始状态（输入处理前的状态）
                AstronautState initialState2 = stateBuffer[(currentStateIndex + 1) % Fnum];
                astronaut.position = initialState2.position;
                astronautCamera.deltaPos = initialState2.cameraPos;
                astronaut.allAngle = initialState2.angles;
                astronautCamera.orientation = initialState2.orientation;
                astronautCamera.UpdateEulerFromOrientation();

            }
            // 更新索引（确保不会超出范围）
            currentStateIndex = (currentStateIndex + 1) % Fnum;


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
