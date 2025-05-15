#include "base.h"
#include "CVector.h"
#include "testFunc.h"
#include "star.h"
#include "planet.h"
#include "ship.h"
#include "CMatrix.h"

void SetRC();
void myReshape(int w, int h);


void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清除颜色和深度缓冲区
    drawPlanet();
    drawAxis();
    drawStars();
    drawShip();
    glutSwapBuffers();
}

void myTimerFunc(int val) {

    autoShip();
    shipMove();
    planetRotation();
    myDisplay();
    checkStar();

    glutTimerFunc(33, myTimerFunc, 0);
}

int main(int argc, char* argv[]) {
    CalculateMatrix();

    glutInit(&argc, argv);// 初始化 GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);// 设置显示模式为双缓冲和 RGB 颜色模式
    glutInitWindowSize(1600, 1200);// 设置窗口大小
    glutInitWindowPosition(100, 100);// 设置窗口位置
    glutCreateWindow("homework 1");// 创建窗口
    glutReshapeFunc(myReshape);//窗口大小改变回调

    initstar();
    initPlanet();
    initShip();

    glutDisplayFunc(&myDisplay);//执行绘制回调
    glutTimerFunc(33, myTimerFunc, 0);//大约30帧
    SetRC();
    glutSpecialFunc(specialKeys); //切换绘制模式
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseClick);//鼠标选择行星
    glutMainLoop();
    return 0;
}

void myReshape(int w,int h) {// 设置视口匹配窗口尺寸
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); // 更新投影矩阵
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void SetRC() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);  // 启用深度测试
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0); // 更合理的默认视角
}


