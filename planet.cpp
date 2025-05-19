#include "CVector.h" 
#include "planet.h" 
#include "base.h"
#include "ship.h"
#include "global.h"
#include "CMatrix.h" 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PLANETNUM 8
#define A2R(x) (x/180.0*PI) //角度转弧度

bool g_wireframe = false;
ball planet[PLANETNUM];//struct ball类型
float radiusCorrection = 1.2, distanceCorrection = 1.1, rotationCorrection = 1.1;//一些参数的修正系数
ball* selectedPlanet = nullptr; // 当前选中的行星

GLuint LoadTexture(const char* path) {

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (!data) {
        printf("加载纹理失败: %s\n", path);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
        (channels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

void drawPlanet() {
    for (int i = 0; i < PLANETNUM; i++) {
        drawTrack(planet[i]);
        drawBall(planet[i]);
        if (planet[i].selected) {//绘制选择效果
            glPushMatrix();
            transMat1.SetTrans(CVector(planet[i].centerPlace.x, planet[i].centerPlace.y, planet[i].centerPlace.z));
            glMultMatrixf(transMat1);
            glColor3f(1,1,1); // 线框
            glutWireSphere(planet[i].r * 1.15, 12, 12);
            glPopMatrix();
        }
    }
    drawRing(planet[6]);
}

void mouseClick(int button, int state, int x, int y) {
    bool successTag = false;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (selectedPlanet != nullptr) {// 清除之前选中的行星
            selectedPlanet->selected = false;
            selectedPlanet = nullptr;
        }
        // 转换鼠标坐标到3D射线
        GLint viewport[4];
        GLdouble modelview[16], projection[16];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);

        GLdouble startX, startY, startZ;
        gluUnProject(x, viewport[3] - y, 0, modelview, projection, viewport, &startX, &startY, &startZ);

        GLdouble endX, endY, endZ;
        gluUnProject(x, viewport[3] - y, 1, modelview, projection, viewport, &endX, &endY, &endZ);
        // 射线与行星碰撞检测
        CVector rayDir(endX - startX, endY - startY, endZ - startZ);
        rayDir.Normalize();

        for (int i = 0; i < PLANETNUM;i++) { // 行星数组
            CVector toCenter = planet[i].centerPlace - CVector(startX, startY, startZ);
            float t = toCenter.dotMul(rayDir);
            if (t < 0) continue; // 射线反向

            CVector nearest = CVector(startX, startY, startZ) + rayDir * t;
            float dist = (nearest - planet[i].centerPlace).len();
            if (dist < planet[i].r) {
                myShip.targetBall = &planet[i];
                successTag = true;
                planet[i].selected = true;
                selectedPlanet = &planet[i];
                break;
            }
        }
        if (!successTag) {
            myShip.targetBall = nullptr;
            if (myShip.speedLen <= myShip.speedStep * 2) {
                myShip.speedLen = myShip.speedTempLen;
            }
        }
    }
}


void initPlanet() {
    planet[0].textureID = LoadTexture("textures/sun.jpg");        // 太阳
    planet[1].textureID = LoadTexture("textures/mercury.jpg");    // 水星
    planet[2].textureID = LoadTexture("textures/venus.jpg");      // 金星
    planet[3].textureID = LoadTexture("textures/earth.jpg");      // 地球
    planet[4].textureID = LoadTexture("textures/mars.jpg");       // 火星
    planet[5].textureID = LoadTexture("textures/jupiter.jpg");    // 木星
    planet[6].textureID = LoadTexture("textures/saturn.jpg");     // 土星
    planet[7].textureID = LoadTexture("textures/moon.jpg");       // 月球

    for (int i = 0; i < PLANETNUM; i++) {
        planet[i].index = i;
    }

    planet[0].color = CVector(1.0f, 0.0f, 0.0f); //太阳
    planet[0].r = 0.6 * radiusCorrection;
    planet[0].rotationSpeed = 0.065 * rotationCorrection*40;
    initBall(planet[0]);

    planet[1].color = CVector(0.0f, 0.0f, 1.0f); //水星
    planet[1].r = 0.1 * radiusCorrection;
    planet[1].rotationSpeed = 0.01 * 40 * rotationCorrection;
    planet[1].orbitSpeed = 0.36 * 4;
    planet[1].orbitRadius = 1.0f * distanceCorrection;
    planet[1].centerPlace = CVector(planet[1].orbitRadius, 0.0f, 0.0f);
    planet[1].orbitCenter = planet[0].centerPlace;
    planet[1].pointNum = 20;
    planet[1].startNum = 5;
    initBall(planet[1]);

    planet[2].color = CVector(1.0f, 1.0f, 0.0f); //金星
    planet[2].r = 0.18 * radiusCorrection;
    planet[2].rotationSpeed = 0.018 * 40 * rotationCorrection;
    planet[2].orbitSpeed = 0.36 * 1.5;
    planet[2].orbitRadius = 1.5f * distanceCorrection;
    planet[2].centerPlace = CVector(planet[2].orbitRadius, 0.0f, 0.0f);
    planet[2].orbitCenter = planet[0].centerPlace;
    planet[2].pointNum = 35;
    planet[2].startNum = 10;
    initBall(planet[2]);

    planet[3].color = CVector(0.0f, 1.0f, 1.0f); //地球
    planet[3].r = 0.2 * radiusCorrection;
    planet[3].rotationSpeed = 0.02 * 40 * rotationCorrection;
    planet[3].orbitSpeed = 0.36;
    planet[3].orbitRadius = 2.0f * distanceCorrection;
    planet[3].centerPlace = CVector(planet[3].orbitRadius, 0.0f, 0.0f);
    planet[3].orbitCenter = planet[0].centerPlace;
    planet[3].pointNum = 45;
    planet[3].startNum = 15;
    initBall(planet[3]);

    planet[4].color = CVector(0.6f, 0.2f, 0.2f); //火星
    planet[4].r = 0.16 * radiusCorrection * rotationCorrection;
    planet[4].rotationSpeed = 0.016 * 40;
    planet[4].orbitSpeed = 0.36 / 2;
    planet[4].orbitRadius = 2.5f * distanceCorrection;
    planet[4].centerPlace = CVector(planet[4].orbitRadius, 0.0f, 0.0f);
    planet[4].orbitCenter = planet[0].centerPlace;
    planet[4].pointNum = 70;
    planet[4].startNum = 20;
    initBall(planet[4]);

    planet[5].color = CVector(0.6f, 0.6f, 0.2f); //木星
    planet[5].r = 0.37 * radiusCorrection * rotationCorrection;
    planet[5].rotationSpeed = 0.037 * 40;
    planet[5].orbitSpeed = 0.36 / 11 * 2;//修正
    planet[5].orbitRadius = 3.1f * distanceCorrection;
    planet[5].centerPlace = CVector(planet[5].orbitRadius, 0.0f, 0.0f);
    planet[5].orbitCenter = planet[0].centerPlace;
    planet[5].pointNum = 280;
    planet[5].startNum = 100;
    initBall(planet[5]);

    planet[6].color = CVector(0.6f, 0.9f, 0.3f); //土星
    planet[6].r = 0.30 * radiusCorrection * rotationCorrection;
    planet[6].rotationSpeed = 0.030 * 40;
    planet[6].orbitSpeed = 0.36 / 30 * 3;//修正
    planet[6].orbitRadius = 4.2f * distanceCorrection;
    planet[6].centerPlace = CVector(planet[6].orbitRadius, 0.0f, 0.0f);
    planet[6].orbitCenter = planet[0].centerPlace;
    planet[6].pointNum = 350;
    planet[6].startNum = 120;
    initBall(planet[6]);

    planet[7].color = CVector(0.6, 0.6, 0.6); //月球
    planet[7].r = 0.05 * radiusCorrection * rotationCorrection;
    planet[7].rotationSpeed = 0.005 * 40;
    planet[7].orbitSpeed = 0.36 * 4;
    planet[7].orbitRadius = 0.3f * distanceCorrection;
    planet[7].centerPlace = CVector(planet[7].orbitRadius + planet[3].orbitRadius, 0.0f, 0.0f);//特殊
    planet[7].orbitCenter = planet[3].centerPlace;//绕地球
    planet[7].pointNum = 25;
    strcpy_s(planet[7].name, "moon");
    initBall(planet[7]);

}

void drawRing(ball saturn) {
    glPushMatrix();// 应用与土星相同的变换
    transMat1.SetTrans(CVector(saturn.orbitPoints[saturn.index].x, saturn.orbitPoints[saturn.index].y, saturn.orbitPoints[saturn.index].z));
    rotateMat1.SetRotate(33.5, CVector(0, 0, 1));// 黄赤交角倾斜
    rotateMat2.SetRotate(saturn.rotationAngle, CVector(0, 1, 0));// 自转角度
    glMultMatrixf(transMat1*rotateMat1*rotateMat2);
    // 环参数设置
    const float innerRadius = saturn.r * 1.6f;  // 内径
    const float outerRadius = saturn.r * 2.1f;  // 外径
    const int segments = 72;                   // 细分段数
    int ringCount = 40;                    // 环层数,也受F1控制

    if (g_wireframe) {
        ringCount = 8;  // 线框模式
    }
    else {
        ringCount = 40;  // 填充模式
    }
    glColor3f(0.8f, 0.8f, 0.6f); // 土星环颜色
    glLineWidth(1.5f);

    for (int r = 0; r < ringCount; ++r) {// 绘制多层环
        float radius = innerRadius + (outerRadius - innerRadius) * r / (ringCount - 1);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; ++i) {
            float theta = 2.0f * PI * i / segments;
            glVertex3f(radius * cos(theta), 0.0f, radius * sin(theta));
        }
        glEnd();
    }

    glPopMatrix();
}

void initBall(ball& ball0) {
    int temp = 4;
    for (int i = -90; i <= 90; i += temp)
    {
        for (int j = 0; j <= 360; j += temp)
        {
            float r = ball0.r * cos(A2R(i));
            ball0.pointPlace[(90 + i) / temp][j / temp].x = r * sin(A2R(j));
            ball0.pointPlace[(90 + i) / temp][j / temp].y = r * cos(A2R(j));
            ball0.pointPlace[(90 + i) / temp][j / temp].z = ball0.r * sin(A2R(i));

            float u = (j) / 360.0f;
            float v = (i + 90.0f) / 180.0f;
            ball0.texCoords[(90 + i) / temp][j / temp] = CVector2(u, v);
        }
    }
    ball0.pointPlace[60][120] = ball0.pointPlace[0][0]; //使用CVerter类的等于

    for (int j = 0; j < 361; j++) {//初始化轨迹
        ball0.orbitPoints[j] = ball0.centerPlace;
    }
}

void planetRotation() {//公转
    for (int i = 0; i < PLANETNUM; i++) {
        planet[i].rotationAngle += planet[i].rotationSpeed;
        planet[i].orbitAngle += planet[i].orbitSpeed;
        if (planet[i].orbitAngle > 360)
            planet[i].orbitAngle -= 360;

        planet[i].index = (planet[i].index + 1) % 360;

        float radian = A2R(planet[i].orbitAngle); // 计算当前世界坐标（核心计算公式）
        planet[i].orbitPoints[planet[i].index] =
            planet[i].orbitCenter + CVector(planet[i].orbitRadius * cos(radian), 0, -planet[i].orbitRadius * sin(radian));
        planet[i].centerPlace = planet[i].orbitPoints[planet[i].index];

        if (i == 3) {
            planet[7].orbitCenter = planet[3].orbitPoints[planet[3].index];// 月球公转中心
        }
    }
}

void drawTrack(ball ball0) {
    // 保留原有的历史轨迹绘制（粗线）
    glPushMatrix();
    glColor3f(1, 1, 1);
    glLineWidth(3.5f);          // 历史轨迹保持粗线
    glBegin(GL_LINE_STRIP);
    int currentIndex = ball0.index;
    for (int i = ball0.startNum; i < ball0.pointNum; i++) {
        int idx = (currentIndex - i + 360) % 360;
        glVertex3fv(ball0.orbitPoints[idx]);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();// 新增完整圆形轨迹（细线）
    glColor3f(0.5f, 0.5f, 1.0f); // 改用淡蓝色区分
    glLineWidth(1.5f);           // 细线设置

    transMat1.SetTrans(CVector(ball0.orbitCenter.x, ball0.orbitCenter.y, ball0.orbitCenter.z));
    glMultMatrixf(transMat1);
    const int segments = 90;    // 减少分段数保持性能
    const float angleIncrement = 2.0f * PI / segments;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float angle = i * angleIncrement;
        float x = ball0.orbitRadius * cos(angle);
        float z = -ball0.orbitRadius * sin(angle);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glPopMatrix();
}

void drawBall(ball ball0) {

    glPushMatrix();
    transMat1.SetTrans(CVector(ball0.orbitPoints[ball0.index].x,
        ball0.orbitPoints[ball0.index].y,
        ball0.orbitPoints[ball0.index].z));
    rotateMat1.SetRotate(33.5, CVector(0, 0, 1));
    rotateMat2.SetRotate(ball0.rotationAngle, CVector(0, 1, 0));
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

    // 启用纹理并绑定
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ball0.textureID);

    // 设置纹理参数
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // 使用平滑着色
    glShadeModel(GL_SMOOTH);

    for (int i = 0; i < 60; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j < 121; j++) {
            // 指定纹理坐标和顶点坐标
            glTexCoord2fv(ball0.texCoords[i][j]);
            glVertex3fv(ball0.pointPlace[i][j]);

            glTexCoord2fv(ball0.texCoords[i + 1][j]);
            glVertex3fv(ball0.pointPlace[i + 1][j]);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D); // 禁用纹理
    glPopMatrix();
}