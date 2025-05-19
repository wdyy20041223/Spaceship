#include "base.h"
#include "CVector.h"
#include "testFunc.h"
#include "star.h"
#include "planet.h"
#include "ship.h"
#include "CMatrix.h"
#include "Camera.h"
#include "keyboard.h"
#include "Astronaut.h"
#include <iostream>


void SetRC();
void myReshape(int w, int h);
void CheckCameraStateChange();

extern Camera globalCamera,astronautCamera,tempCamera;  // 全局摄像机对象
extern bool ControlingGlobal;


void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 绘制3D场景
    drawAstronaut();
    drawPlanet();
    drawAxis();
    drawStars();
    drawShip();


    // 在3D场景绘制完成后，显示摄像机信息（作为HUD）
    if (globalCamera.online) {
        globalCamera.RenderInfo("Global");  // 传递视角类型
    }
    else {
        astronautCamera.RenderInfo("Astronaut");  // 传递视角类型
    }
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    CVector pos;
    CVector forward;
    CVector up;
    // 设置观察点为目标点（相机位置+前向方向）
    CVector target;


    //太空人相机同步
    forward = astronautCamera.GetForwardDir().Normalized();
    pos = astronaut.head;
    up = astronautCamera.GetUpDir();
    target = pos + forward;
    pos = pos - forward * (0.05) + (myShip.speedLen) * myShip.direction;

    astronautCamera.position = pos;

    if (globalCamera.transition.isActive || astronautCamera.transition.isActive) {

        pos = tempCamera.position;
        forward = tempCamera.GetForwardDir();
        up = tempCamera.GetUpDir();
        // 设置观察点为目标点（相机位置+前向方向）
        target = pos + forward;
    }
    else
    if (globalCamera.online == true) {
        // 使用globalCamera实时位置
        pos = globalCamera.position;
        forward = globalCamera.GetForwardDir();
        up = globalCamera.GetUpDir();
        // 设置观察点为目标点（相机位置+前向方向）
        target = pos + forward;       
    }
    else {
        astronautCamera.moveSpeed = myShip.speedLen;        
    }
    gluLookAt(
        pos.x, pos.y, pos.z,    // 相机位置
        target.x, target.y, target.z, // 观察点
        up.x, up.y, up.z        // 上方向
    );
}

void myTimerFunc(int val) {
    float deltaTime = 33.0f / 1000.0f; // 33ms per frame

    // 更新相机过渡状态
    bool globalCompleted = globalCamera.UpdateTransition(deltaTime);
    bool astronautCompleted = astronautCamera.UpdateTransition(deltaTime);
    // 处理过渡完成后的控制权切换
    if (globalCompleted) {
        globalCamera.online = false;
        astronautCamera.online = true;
        ControlingGlobal = false;
    }
    if (astronautCompleted) {
        astronautCamera.online = false;
        globalCamera.online = true;
        ControlingGlobal = true;
    }

    checkKeyStates();

    autoShip();
    shipMove();
    planetRotation();
    myDisplay();
    checkStar();

    //五帧刷新
    static int frameCount = 0;
    if (frameCount % 5 == 0) {
        CheckCameraStateChange();
        frameCount = 0; 
    }
    frameCount++;
    
    glutTimerFunc(33, myTimerFunc, 0);
}

int main(int argc, char* argv[]) {
    testfunc();
    CalculateTest();

    glutInit(&argc, argv);// 初始化 GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);// 设置显示模式为双缓冲和 RGB 颜色模式
    glutInitWindowSize(1600, 1200);// 设置窗口大小
    glutInitWindowPosition(100, 100);// 设置窗口位置
    glutCreateWindow("homework 1");// 创建窗口

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }

    glutReshapeFunc(myReshape);//窗口大小改变回调

    initstar();
    initPlanet();
    initShip();
    initCamera();
    initAstronaut();
    
    glutDisplayFunc(&myDisplay);//执行绘制回调
    glutTimerFunc(33, myTimerFunc, 0);//大约30帧
    SetRC();

    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    glutMouseFunc(mouseClick);//鼠标选择行星
    glutMainLoop();
    return 0;
}

void myReshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 0.04, 100.0);

}

void SetRC() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);  // 启用深度测试
    glMatrixMode(GL_MODELVIEW);
}


void CheckCameraStateChange() {
    const Camera& cam = ControlingGlobal ? globalCamera : astronautCamera;
    const float EPSILON = 0.001f;
    bool changed = false;

    // 位置变化检测（使用 len()）
    if ((cam.position - g_lastCamPos).len() > EPSILON) {
        changed = true;
        g_lastCamPos = cam.position;
    }

    // 欧拉角变化检测
    CEuler currentEuler = cam.GetEulerAngles();
    if (fabs(currentEuler.h - g_lastCamEuler.h) > EPSILON ||
        fabs(currentEuler.p - g_lastCamEuler.p) > EPSILON ||
        fabs(currentEuler.b - g_lastCamEuler.b) > EPSILON) {
        changed = true;
        g_lastCamEuler = currentEuler;
    }

    // 方向向量变化检测
    CVector currentForward = cam.GetForwardDir();
    CVector currentUp = cam.GetUpDir();
    if ((currentForward - g_lastCamForward).len() > EPSILON) {
        changed = true;
        g_lastCamForward = currentForward;
    }
    if ((currentUp - g_lastCamUp).len() > EPSILON) {
        changed = true;
        g_lastCamUp = currentUp;
    }

    // 速度变化检测
    if (fabs(cam.moveSpeed - g_lastCamSpeed) > EPSILON) {
        changed = true;
        g_lastCamSpeed = cam.moveSpeed;
    }

    // 模式变化检测
    if (cam.currentMode != g_lastCamMode) {
        changed = true;
        g_lastCamMode = cam.currentMode;
    }

    // 打印变化信息
    if (changed) {
        std::cout << "\n[Camera State]\n";  // 单独一行标题
        std::cout << "Position: (" << cam.position.x << ", " << cam.position.y << ", " << cam.position.z << ")\n";
        std::cout << "Rotation: H=" << currentEuler.h << "°  P=" << currentEuler.p << "°  B=" << currentEuler.b << "°\n";
        std::cout << "Forward:  (" << currentForward.x << ", " << currentForward.y << ", " << currentForward.z << ")\n";
        std::cout << "Up:       (" << currentUp.x << ", " << currentUp.y << ", " << currentUp.z << ")\n";
        std::cout << "Speed:    " << cam.moveSpeed << " units/sec\n";
        std::cout << "Mode:     " << (g_lastCamMode == Camera::EULER ? "Euler" : "Local") << "\n\n";
    }
}