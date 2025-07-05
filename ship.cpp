
#include "base.h"
#include "CMatrix.h"
#include "planet.h"


extern Camera globalCamera, astronautCamera, shipCamera;
extern bool ControllingShip;

void shipMove() {
    myShip.direction = getShipDir(myShip);
    myShip.speed = myShip.direction * myShip.speedLen;
    myShip.position = myShip.position + myShip.speed;
    astronautCamera.origonPos = astronautCamera.origonPos + myShip.speed;
    shipCamera.origonPos = shipCamera.origonPos + myShip.speed;
}
CVector getForwardDirection(const CQuaternion& orientation) {
    // 标准前方向量（假设初始朝向为Z轴负方向）
    CVector baseForward(0, 0, -1);
    return orientation * baseForward;  // 四元数作用于向量
}
CVector getUpDirection(const CQuaternion& orientation) {
    // 标准上方向量（Y轴正方向）
    CVector baseUp(0, 1, 0);
    return orientation * baseUp;
}
// 获取右侧方向
CVector getRightDirection(const CQuaternion& orientation) {
    CVector baseRight(1, 0, 0);
    return orientation * baseRight;
}


void autoShip() {
    if (myShip.autoPilot && myShip.targetBall != nullptr) {
        ball targrt = *(myShip.targetBall);
        float dist = myShip.position.distanceTo(targrt.centerPlace);


        if (dist > targrt.r * 1.5) {

            CVector finalDir = (targrt.centerPlace - myShip.position).Normalized();
            float angle = myShip.position.angleWith(finalDir);
            float yaw = atan2(finalDir.x, finalDir.z) * 180.0f / PI;
            float pitch = asin(finalDir.y) * 180.0f / PI;

            CVector tempdir1 = myShip.direction.Normalized();
            CQuaternion q1 = tempdir1.ToEuler().ToQuaternion(); // 当前方向的四元数
            CQuaternion q2 = finalDir.ToEuler().ToQuaternion(); // 目标方向的四元数

            // 计算从当前到目标的旋转四元数
            CQuaternion deltaQ = q2 * q1.Inverse();

            // 应用旋转到相机
            astronautCamera.orientation = deltaQ * astronautCamera.orientation;
            astronautCamera.orientation.Normalize();
            astronautCamera.UpdateEulerFromOrientation(); // 同步欧拉角

            shipCamera.orientation = deltaQ * astronautCamera.orientation;
            shipCamera.orientation.Normalize();
            shipCamera.UpdateEulerFromOrientation(); // 同步欧拉角

            myShip.orientation = deltaQ * myShip.orientation;

            if (myShip.speedLen <= myShip.speedStep * 2 && myShip.speedTempLen != 0) {
                myShip.speedLen = myShip.speedTempLen;
                myShip.speedTempLen = 0;
            }
        }
        else {
            if (myShip.speedLen >= myShip.speedStep * 2)
                myShip.speedTempLen = myShip.speedLen;
            myShip.speedLen = 0.0f;
        }

    }
}


void drawAxis() {
    CVector tempColor;
    glLineWidth(2.0f); // 设置线段宽度为2.0

    tempColor.Set(1.0f, 0.0f, 0.0f);// 设置颜色为红色 x
    glColor3fv(tempColor);
    glBegin(GL_LINES); // 开始绘制线段
    glVertex3f(0.0f, 0.0f, 0.0f); // X轴起点
    glVertex3f(4.5f, 0.0f, 0.0f); // X轴终点
    glEnd();

    glPushMatrix();//画圆锥
    transMat1.SetTrans(CVector(4.4, 0, 0));
    rotateMat1.SetRotate(90, CVector(0, 1, 0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();

    tempColor.Set(0.0f, 1.0f, 0.0f);// 设置颜色为绿色 y 
    glColor3fv(tempColor);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f); // Y轴起点
    glVertex3f(0.0f, 3.0f, 0.0f); // Y轴终点
    glEnd();

    glPushMatrix();//画圆锥
    transMat1.SetTrans(CVector(0, 2.9, 0));
    rotateMat1.SetRotate(-90, CVector(1, 0, 0));
    glMultMatrixf(transMat1 * rotateMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();

    tempColor.Set(0.0f, 0.0f, 1.0f);// 设置颜色为蓝色 z
    glColor3fv(tempColor);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f); // Z轴起点
    glVertex3f(0.0f, 0.0f, 4.5f); // Z轴终点
    glEnd();

    glPushMatrix();//画圆锥
    transMat1.SetTrans(CVector(0, 0, 4.4));
    glMultMatrixf(transMat1);
    glutSolidCone(0.05f, 0.1f, 36, 10);
    glPopMatrix();

}

void drawSeat(float len, float pos1, float pos2) {
    glPushMatrix();
    {
        // 设置座位的材质属性
        GLfloat mat_ambient[] = { 0.6f, 0.6f, 0.f, 1.0f };   // 环境光
        GLfloat mat_diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };   // 漫反射
        GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };  // 镜面反射
        GLfloat mat_shininess = 80.0f;                        // 光泽度

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

        // 应用变换：平移 + 旋转 + 缩放
        transMat1.SetTrans(CVector(pos1, -0.185f, pos2));
        rotateMat1.SetRotate(180.0f, CVector(0, 1, 0)); // 绕Y轴旋转180度
        scaleMat1.SetScale(CVector(len, 0.5f, 0.51f));
        glMultMatrixf(scaleMat1 * rotateMat1 * transMat1);

        // ============= 座垫部分 =============
        scaleMat1.SetScale(CVector(0.5f, 0.1f, 0.2f));
        glPushMatrix();
        glMultMatrixf(scaleMat1);

        // 启用座位纹理
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.seatTexture);
        glColor3f(1.0f, 1.0f, 1.0f); // 设置白色基础色

        // 绘制立方体并指定法线和纹理坐标
        glBegin(GL_QUADS);

        // 前面（法线朝前）
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        // 后面（法线朝后）
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        // 上面（法线朝上）
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        // 下面（法线朝下）
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        // 左面（法线朝左）
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

        // 右面（法线朝右）
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // ============= 靠背部分 =============
        transMat1.SetTrans(CVector(0.0f, 0.1f, 0.1f));
        rotateMat1.SetRotate(10.0f, CVector(1, 0, 0)); // 轻微倾斜
        scaleMat1.SetScale(CVector(0.5f, 0.3f, 0.05f));
        glPushMatrix();
        glMultMatrixf(transMat1 * rotateMat1 * scaleMat1);

        // 启用座位纹理
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.seatTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        // 增强靠背材质效果
        GLfloat back_mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
        GLfloat back_mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, back_mat_ambient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, back_mat_specular);

        // 绘制靠背立方体
        glBegin(GL_QUADS);

        // 前面（法线朝前）
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        // 后面（法线朝后）
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        // 上面（法线朝上）
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        // 下面（法线朝下）
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        // 左面（法线朝左）
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

        // 右面（法线朝右）
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
    glPopMatrix();
}

void initShip() {
    ControllingShip = true;
    myShip.angleStep = 1.8;
    myShip.position = CVector(2, 2, 2);
    float a = 0.2, test = 1;
    myShip.speedLen = 0.01 * a;
    myShip.speedStep = 0.002 * a * test;
    myShip.autoPilot = false;


    myShip.seatTexture = LoadTexture("textures/seat.jpg");       // 座椅皮革纹理
    myShip.bodyTexture = LoadTexture("textures/ship_body.jpg");  // 船体金属纹理
    myShip.engineTexture = LoadTexture("textures/ship_body.jpg");  // 引擎灼烧纹理
    myShip.wingTexture = LoadTexture("textures/ship_body.jpg"); // 机翼装甲
    myShip.glassTexture = LoadTexture("textures/ship_body.jpg"); // 玻璃效果
    myShip.panelTexture = LoadTexture("textures/ship_body.jpg"); // 控制面板
    myShip.envMapTexture = LoadTexture("textures/ship_body.jpg"); // 环境反射贴图
    myShip.floorTexture = LoadTexture("textures/ship_body.jpg");
}



void drawShip() {
    glEnable(GL_LIGHTING); // 确保光照计算开启
    glEnable(GL_NORMALIZE); // 自动法线归一化
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);

    //glDisable(GL_LIGHT0);
    //glDisable(GL_LIGHT2);
    //glDisable(GL_LIGHT3);

    glPushMatrix();
    // 使用四元数生成旋转矩阵
    CMatrix rotateMat = myShip.orientation.ToMatrix();

    // 组合变换矩阵
    transMat1.SetTrans(myShip.position);
    scaleMat1.SetScale(CVector(0.2f, 0.2f, 0.2f));
    finalMat = transMat1 * rotateMat * scaleMat1;
    glMultMatrixf(finalMat);

    glColor3f(0.0, 0.0, 1.0);//顶部标记（驾驶舱）
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0.35, 0));
    glMultMatrixf(transMat1);
    glutSolidSphere(0.2, 4, 4);
    glPopMatrix();

    glPushMatrix();
    {
        // 设置深蓝色金属材质
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };   // 环境光（浅灰）
        GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // 漫反射（纯白）
        GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 镜面反射（纯白）
        GLfloat mat_shininess = 128.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


        GLUquadricObj* quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluQuadricNormals(quadric, GLU_SMOOTH);

        transMat1.SetTrans(CVector(0, 0, -0.65f));
        glMultMatrixf(transMat1);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myShip.bodyTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // 移除蓝色调叠加，改为白色
        glColor3f(1.0f, 1.0f, 1.0f);  // 纯白
        gluCylinder(quadric, 0.25f, 0.25f, 1.3f, 64, 64);

        // 修改底面圆盘材质
        GLfloat disk_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, disk_specular);
        glColor3f(0.8f, 0.8f, 0.8f);  // 浅灰色
        gluDisk(quadric, 0.0f, 0.25f, 48, 1);

        gluDeleteQuadric(quadric);
        glDisable(GL_TEXTURE_2D);
    }



    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, 1.3f));
    glMultMatrixf(transMat1);
    // ============= 修正后的内部装饰物绘制 =============
    glPushMatrix();
    {
        // 全局缩放（注意矩阵顺序应为缩放*原始变换）
        scaleMat1.SetScale(CVector(0.5f, 0.5f, 0.5f));
        glMultMatrixf(scaleMat1);

        //----- 控制台仪表盘 -----
        glDisable(GL_LIGHT0);
        glPushMatrix();
        {   
            GLfloat metal_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
            GLfloat metal_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
            GLfloat metal_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
            GLfloat metal_shininess = 128.0f;

            glMaterialfv(GL_FRONT, GL_AMBIENT, metal_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, metal_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, metal_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, metal_shininess);

            // 平移->旋转组合（注意OpenGL矩阵右乘规则）
            transMat1.SetTrans(CVector(0.0f, -0.105f, -0.4f));
            rotateMat1.SetRotate(180.0f, CVector(0, 1, 0)); // 修正旋转轴为(0,1,0)
            rotateMat2.SetRotate(5.0f, CVector(1, 0, 0));
            glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

            // 仪表盘基座（缩放应用在局部坐标系）
            scaleMat1.SetScale(CVector(0.8f, 0.05f, 0.3f));
            glPushMatrix();
            glMultMatrixf(scaleMat1);
            glColor3f(0.2f, 0.2f, 0.2f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // 屏幕材质（亚克力效果）
            GLfloat screen_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
            GLfloat screen_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT, screen_ambient);
            glMaterialfv(GL_FRONT, GL_SPECULAR, screen_specular);

            // 液晶屏幕（先平移后缩放）
            transMat1.SetTrans(CVector(0.0f, 0.03f, 0.1f));
            scaleMat1.SetScale(CVector(0.7f, 0.03f, 0.2f));
            glPushMatrix();
            glMultMatrixf(transMat1 * scaleMat1); // 等价于先缩放再平移的逆序
            glColor3f(0.1f, 0.1f, 0.1f);
            glutSolidCube(1.0f);
            glPopMatrix();

            GLfloat button_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
            GLfloat button_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT, button_ambient);
            glMaterialfv(GL_FRONT, GL_SPECULAR, button_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

            // 操作按钮（独立平移）
            glColor3f(0.0f, 0.6f, 0.0f);
            glPushMatrix();
            transMat1.SetTrans(CVector(0.3f, 0.03f, -0.1f));
            glMultMatrixf(transMat1);
            glutSolidSphere(0.03f, 8, 8);
            transMat1.SetTrans(CVector(-0.6f, 0.0f, 0.0f));
            glMultMatrixf(transMat1);
            glutSolidSphere(0.03f, 8, 8);
            glPopMatrix();
        }
        glPopMatrix();

        glEnable(GL_LIGHT2);
        glEnable(GL_LIGHT3);
        drawSeat(0.5, 0, 1.5);//pilot len,pos1,pos2
        drawSeat(1.91, 0, 4);//behind
        drawSeat(0.5f, -0.7, 2);//l1
        drawSeat(0.5f, -0.7, 2.5);//l2
        drawSeat(0.5f, -0.7, 3);//l3
        drawSeat(0.5f, -0.7, 3.5);//l4
        drawSeat(0.5f, 0.7, 2);//r1
        drawSeat(0.5f, 0.7, 2.5);//r2
        drawSeat(0.5f, 0.7, 3);//rl3
        drawSeat(0.5f, 0.7, 3.5);//r4
        
    }
    glPopMatrix();
    glEnable(GL_LIGHT0);

    GLfloat glass_ambient[] = { 0.1f, 0.1f, 0.1f, 0.5f };
    GLfloat glass_diffuse[] = { 0.6f, 0.6f, 0.6f, 0.5f };
    GLfloat glass_specular[] = { 0.9f, 0.9f, 0.9f, 0.5f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, glass_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glass_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glass_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 96.0f);

    // 修改开始：头部圆锥半透明处理
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT); // 保存状态
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6, 0.4, 0.7, 0.5); // 第4个参数0.5表示50%透明度
    glutSolidCone(0.25f, 0.6f, 64, 64);
    glDisable(GL_BLEND);
    glPopAttrib(); // 恢复原始状态
    // 修改结束

    glPopMatrix();

    glPopMatrix();

    // 甲板 - 添加纹理
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myShip.floorTexture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // 设置地板材质
    GLfloat floor_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat floor_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat floor_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    glPushMatrix();
    transMat1.SetTrans(CVector(0, -0.055, 0));
    scaleMat1.SetScale(CVector(0.475, 0.0002, 1.358));
    glMultMatrixf(transMat1* scaleMat1);

    // 绘制带纹理和法线的地板
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // 法线向上
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_LIGHT0);
    glColor3f(1.0, 0.5, 0.0);// 引擎喷口
    glPushMatrix();
    transMat1.SetTrans(CVector(0, 0, -0.8f));
    glMultMatrixf(transMat1);
    glutSolidCone(0.2, 0.4, 8, 4);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);  // 启用纹理
    glBindTexture(GL_TEXTURE_2D, myShip.wingTexture);  // 绑定机翼纹理

    glEnable(GL_LIGHT0);

    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
    DrawWing(true);
    // 左机翼（自动纹理+法线）
    glEnable(GL_LIGHT0);
    DrawWing(false);

    glDisable(GL_TEXTURE_2D);  // 禁用纹理

    glPopMatrix();

    glDisable(GL_LIGHTING);
}

// 绕局部Y轴旋转（偏航）
void ShipYaw(float angle) {
    CVector localUp = myShip.orientation * CVector(0, 1, 0); // 获取局部上方向
    CQuaternion rot;
    rot.SetAngle(angle, localUp);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

// 绕局部X轴旋转（俯仰）
void ShipPitch(float angle) {
    CVector localRight = myShip.orientation * CVector(-1, 0, 0); // 获取局部右方向
    CQuaternion rot;
    rot.SetAngle(angle, localRight);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

// 绕局部Z轴旋转（滚转）
void ShipRoll(float angle) {
    CVector localForward = myShip.orientation * CVector(0, 0, -1); // 获取局部前方向
    CQuaternion rot;
    rot.SetAngle(angle, localForward);
    myShip.orientation = rot * myShip.orientation;
    myShip.orientation.Normalize();
}

CVector getShipDir(ship myShip) {
    return (myShip.orientation * CVector(0, 0, 1)).Normalized();
}

void DrawWing(bool isRightWing) {
    glPushMatrix();

    // ============= 自动纹理生成设置 =============
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    // 根据机翼尺寸设置纹理坐标生成参数
    GLfloat s_plane[4] = { 2.0f, 0.0f, 0.0f, 0.0f };
    GLfloat t_plane[4] = { 0.0f, 0.0f, 2.0f, 0.0f };
    glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane);

    // ============= 材质设置 =============
    GLfloat metal_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat metal_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat metal_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, metal_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, metal_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, metal_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0f);

    // ============= 几何变换 =============
    float wingSign = isRightWing ? 1.0f : -1.0f; // 左右翼符号

    // 不再使用glScalef进行镜像，而是直接设置位置
    transMat1.SetTrans(CVector(wingSign * 0.65f, 0, 0));
    scaleMat1.SetScale(CVector(0.8f, 0.1f, 0.5f));
    glMultMatrixf(transMat1 * scaleMat1);

    // ============= 带法线的四边形绘制 =============
    glBegin(GL_QUADS);

    // 前表面（法线沿Z正方向）
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // 后表面（法线沿Z负方向）
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // 上表面（法线沿Y正方向）
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // 下表面（法线沿Y负方向）
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // 侧表面（根据左右翼调整法线方向）
    // 使用wingSign确保法线始终指向外侧
    glNormal3f(wingSign, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    glEnd();

    // ============= 清理状态 =============
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();
}