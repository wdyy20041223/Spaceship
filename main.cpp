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
void init();


extern Camera globalCamera,astronautCamera,tempCamera;  // 全局摄像机对象
extern bool ControlingGlobal;


void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // 重置矩阵

    //---------- 以下是新增/修改的关键部分 ----------
    // 1. 先设置观察矩阵
    CVector pos, forward, up, target;

    // 计算相机参数（保持原有逻辑）

    // 太空人视角
    forward = astronautCamera.GetForwardDir().Normalized();
    pos = astronaut.head - forward * 0.025 + myShip.direction * myShip.speedLen;
    up = astronautCamera.GetUpDir();
    target = pos + forward;
    astronautCamera.position = pos;

    if (globalCamera.transition.isActive || astronautCamera.transition.isActive) {
        pos = tempCamera.position;
        forward = tempCamera.GetForwardDir();
        up = tempCamera.GetUpDir();
        target = pos + forward;
    }
    else if (globalCamera.online) {
        pos = globalCamera.position;
        forward = globalCamera.GetForwardDir();
        up = globalCamera.GetUpDir();
        target = pos + forward;
    }
        

    // 然后设置视图矩阵（相机视角）
    gluLookAt(pos.x, pos.y, pos.z,
        target.x, target.y, target.z,
        up.x, up.y, up.z);


    // 3. 绘制3D场景（必须在设置矩阵和光源之后）
    drawPlanet();
    drawShip();
    drawAstronaut();
    drawAxis();
    drawStars();

    // 4. 最后绘制HUD（保持原有逻辑）
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int currentWinWidth = glutGet(GLUT_WINDOW_WIDTH);
    int currentWinHeight = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, currentWinWidth, 0, currentWinHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    if (globalCamera.online) {
        globalCamera.RenderInfo("Global");
    }
    else {
        astronautCamera.RenderInfo("Astronaut");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
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

    // 设置光源位置
    GLfloat light_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

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
    init();

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

void init() {
    // 添加全局环境光设置
    GLfloat global_ambient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // 启用双面光照
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    // 在初始化函数中添加
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // 启用0号光源

    GLfloat light_ambient[] = { 1.4f, 1.4f, 1.4f, 1.0f };  // 适当的环境光
    GLfloat light_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };  // 强漫反射
    GLfloat light_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f }; // 镜面反射

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // 禁用衰减（确保全场景光照）
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

}

void myReshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 0.01, 100.0);

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