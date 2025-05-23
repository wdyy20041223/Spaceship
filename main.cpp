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


extern Camera globalCamera,astronautCamera,tempCamera, shipCamera;  // 全局摄像机对象
extern bool ControlingGlobal;


void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // 重置矩阵

    //---------- 以下是新增/修改的关键部分 ----------
    // 1. 先设置观察矩阵
    CVector pos, forward, up, target;
    CVector astronautPos, astronautForward, astronautUp, astronautTarget;
    CVector shipPos, shipForward, shipUp, shipTarget;

    //全局相机
    globalCamera.position = globalCamera.origonPos;
    // 太空人视角
    astronautForward = astronautCamera.GetForwardDir().Normalized();
    astronautCamera.position = astronautCamera.origonPos - astronautForward * 0.025;
    astronautPos = astronautCamera.position;
    astronautUp = astronautCamera.GetUpDir();
    astronautTarget = astronautPos + astronautForward;
    //astronautCamera.position = astronautPos;
    astronautCamera.moveSpeed = myShip.speedLen;
    // 飞船视角
    shipForward = shipCamera.GetForwardDir().Normalized();
    shipCamera.position = shipCamera.origonPos + shipForward * 0.248;
    shipPos = shipCamera.position ;
    shipUp = shipCamera.GetUpDir();
    shipTarget = shipPos + shipForward;
    shipCamera.moveSpeed = myShip.speedLen;

    if (globalCamera.transition.isActive || astronautCamera.transition.isActive || shipCamera.transition.isActive) {
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
    else if (astronautCamera.online) {
        pos = astronautPos;
        up = astronautUp;
        target = astronautTarget;
    }
    else {
        pos = shipPos;
        up = shipUp;
        target = shipTarget;
    }

    gluLookAt(pos.x, pos.y, pos.z,
        target.x, target.y, target.z,
        up.x, up.y, up.z);

    drawPlanet();
    drawShip();
    drawAstronaut();
    drawAxis();
    drawStars();

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
        globalCamera.OptionInfo("Global");
    }
    else if(astronautCamera.online){
        astronautCamera.RenderInfo("Astronaut");
        astronautCamera.OptionInfo("Astronaut");
    }
    else{
        shipCamera.RenderInfo("Ship");
        shipCamera.OptionInfo("Ship");
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

    //更新相机过渡状态
    bool globalCompleted = globalCamera.UpdateTransition(deltaTime);
    bool astronautCompleted = astronautCamera.UpdateTransition(deltaTime);
    bool shipCompleted = shipCamera.UpdateTransition(deltaTime);

    // 设置光源位置
    GLfloat light_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    CVector tempPos = CVector(1.0, 0, 1.0);
    GLfloat lightPos[] = { tempPos.x, tempPos.y,
                        tempPos.z, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);


    // 更新聚光灯参数
    float len = 0.1;
    GLfloat lightPos2[] = {(myShip.position + myShip.direction * len).x, (myShip.position + myShip.direction * len).y,
                         (myShip.position + myShip.direction * len).z, 1.0f};
    GLfloat spotDir2[] = { myShip.direction.x,myShip.direction.y,myShip.direction.z};
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir2);


    checkKeyStates();

    autoShip();
    shipMove();
    planetRotation();
    myDisplay();
    checkStar();

    //五帧刷新
    static int frameCount = 0;
    if (frameCount % 5 == 0) {
        //CheckCameraStateChange();
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

    init();
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
    myShip.direction = CVector(0, 1, 0);
    // 添加全局环境光设置
    GLfloat global_ambient[] = { 0.2f,0.2f,0.2f, 1.0f };  // 原为0.6
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // 启用双面光照
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    // 在初始化函数中添加
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // 启用0号光源

    // 太阳光源位置应设置为无限远方向光
    GLfloat sun_position[] = {
        0,0,0,
        0.0f  // 重要！0.0表示平行光，非定位光源
    };

    glLightfv(GL_LIGHT0, GL_POSITION, sun_position);
    // 更符合真实太阳光照比例
    GLfloat sun_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // 环境光保持较低
    GLfloat sun_diffuse[] = { 1,1,1.0f, 1.0f };  // 增加亮度20%
    GLfloat sun_specular[] = { 1.5f, 1.5f, 1.2f, 1.0f }; // 增强镜面高光

    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_specular);

    glEnable(GL_LIGHT1);

    // 聚光灯参数配置
    GLfloat light_diffuse2[] = { 1,1,1.0f, 1.0f }; // 降低20%亮度
    GLfloat light_ambient2[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // 弱环境光
    GLfloat light_specular2[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // 适量镜面反射

    // 衰减系数（根据飞船尺寸调整）
    GLfloat att_constant = 1.0f;  // 基础衰减
    GLfloat att_linear = 0.15f;   // 线性衰减系数
    GLfloat att_quadratic = 0.03f;// 二次衰减系数

    // 聚光特性
    GLfloat spot_cutoff = 45.0f;  // 截止角度
    GLfloat spot_exponent = 10.0f; // 光锥集中度

    // 设置基础光照参数
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);

    // 配置衰减系数（重要！）
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);

    // 聚光灯特性设置
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_cutoff);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spot_exponent);
    
}

void myReshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 0.01, 100.0);

}

void SetRC() {
    float color = 0.00f;
    glClearColor(color, color, color, 1.0f);
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