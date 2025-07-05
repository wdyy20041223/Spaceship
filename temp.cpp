//void drawShip() {
//    glPushMatrix();
//    // 主变换矩阵
//    CMatrix rotateMat = myShip.orientation.ToMatrix();
//    transMat1.SetTrans(myShip.position);
//    scaleMat1.SetScale(CVector(0.2f, 0.2f, 0.2f));
//    glMultMatrixf(transMat1 * rotateMat * scaleMat1);
//
//    glPushMatrix();
//    {
//        transMat1.SetTrans(CVector(0, 0, 1.3f));  // 调整到正确位置
//        glMultMatrixf(transMat1);
//        // 驾驶员座位
//        drawSeat(0.5, 0, 1.5);
//        // 后排座位
//        drawSeat(1.91, 0, 4);
//        // 左侧座位组
//        drawSeat(0.5f, -0.7, 2);
//        drawSeat(0.5f, -0.7, 2.5);
//        drawSeat(0.5f, -0.7, 3);
//        drawSeat(0.5f, -0.7, 3.5);
//
//        // 右侧座位组
//        drawSeat(0.5f, 0.7, 2);
//        drawSeat(0.5f, 0.7, 2.5);
//        drawSeat(0.5f, 0.7, 3);
//        drawSeat(0.5f, 0.7, 3.5);
//    }
//    glPopMatrix();
//
//    //-- 驾驶舱玻璃（环境反射+透明效果）--
//    glPushMatrix();
//    {
//        glEnable(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, myShip.glassTexture);
//        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//
//        // 启用球面反射纹理坐标生成
//        glEnable(GL_TEXTURE_GEN_S);
//        glEnable(GL_TEXTURE_GEN_T);
//        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//
//        glPushAttrib(GL_ENABLE_BIT);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//        transMat1.SetTrans(CVector(0, 0.35, 0));
//        glMultMatrixf(transMat1);
//
//        // 多层玻璃效果
//        glColor4f(0.6, 0.4, 0.7, 0.4);
//        glutSolidSphere(0.2, 32, 32);
//
//        glDisable(GL_BLEND);
//        glPopAttrib();
//        glDisable(GL_TEXTURE_GEN_S);
//        glDisable(GL_TEXTURE_GEN_T);
//        glDisable(GL_TEXTURE_2D);
//    }
//    glPopMatrix();
//
//    //-- 主船体（静态纹理）--
//    glPushMatrix();
//    {
//        GLUquadricObj* quadric = gluNewQuadric();
//        gluQuadricTexture(quadric, GL_TRUE);
//
//        transMat1.SetTrans(CVector(0, 0, -0.65f));
//        glMultMatrixf(transMat1);
//
//        glEnable(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, myShip.bodyTexture);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//
//        // 圆柱主体（移除动态纹理）
//        glColor3f(1.0f, 1.0f, 1.0f);
//        gluCylinder(quadric, 0.25f, 0.25f, 1.3f, 16, 4);
//
//        // 底部封口（引擎喷口）
//        glPushMatrix();
//        glBindTexture(GL_TEXTURE_2D, myShip.engineTexture);
//        glRotatef(180, 1, 0, 0);
//        gluDisk(quadric, 0.0f, 0.25f, 18, 1);
//        glPopMatrix();
//
//        gluDeleteQuadric(quadric);
//        glDisable(GL_TEXTURE_2D);
//    }
//    glPopMatrix();
//
//    //-- 甲板（带细节纹理）--
//    glPushMatrix();
//    {
//        transMat1.SetTrans(CVector(0, -0.055, 0));
//        scaleMat1.SetScale(CVector(0.475, 0.0002, 1.258));
//        glMultMatrixf(transMat1 * scaleMat1);
//
//        glEnable(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, myShip.panelTexture);
//
//        // 设置自动纹理坐标
//        glEnable(GL_TEXTURE_GEN_S);
//        glEnable(GL_TEXTURE_GEN_T);
//        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//        GLfloat s_coeff[4] = { 4.0f, 0.0f, 0.0f, 0.0f };
//        GLfloat t_coeff[4] = { 0.0f, 4.0f, 0.0f, 0.0f };
//        glTexGenfv(GL_S, GL_OBJECT_PLANE, s_coeff);
//        glTexGenfv(GL_T, GL_OBJECT_PLANE, t_coeff);
//
//        glColor3f(0.9f, 0.9f, 0.9f);
//        glutSolidCube(1.0);
//
//        glDisable(GL_TEXTURE_GEN_S);
//        glDisable(GL_TEXTURE_GEN_T);
//        glDisable(GL_TEXTURE_2D);
//    }
//    glPopMatrix();
//
//    //-- 机翼（完整纹理坐标）--
//    for (int i = -1; i <= 1; i += 2) {
//        glPushMatrix();
//        transMat1.SetTrans(CVector(0.638 * i, 0, 0));
//        scaleMat1.SetScale(CVector(0.8, 0.1, 0.5));
//        glMultMatrixf(transMat1 * scaleMat1);
//
//        glEnable(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, myShip.wingTexture);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//
//        // 完整立方体纹理坐标
//        glBegin(GL_QUADS);
//        // 前面
//        glTexCoord2f(0, 0); glVertex3f(-0.5, -0.5, 0.5);
//        glTexCoord2f(1, 0); glVertex3f(0.5, -0.5, 0.5);
//        glTexCoord2f(1, 1); glVertex3f(0.5, 0.5, 0.5);
//        glTexCoord2f(0, 1); glVertex3f(-0.5, 0.5, 0.5);
//        // 后面
//        glTexCoord2f(1, 0); glVertex3f(-0.5, -0.5, -0.5);
//        glTexCoord2f(1, 1); glVertex3f(-0.5, 0.5, -0.5);
//        glTexCoord2f(0, 1); glVertex3f(0.5, 0.5, -0.5);
//        glTexCoord2f(0, 0); glVertex3f(0.5, -0.5, -0.5);
//        // 顶面
//        glTexCoord2f(0, 1); glVertex3f(-0.5, 0.5, -0.5);
//        glTexCoord2f(0, 0); glVertex3f(-0.5, 0.5, 0.5);
//        glTexCoord2f(1, 0); glVertex3f(0.5, 0.5, 0.5);
//        glTexCoord2f(1, 1); glVertex3f(0.5, 0.5, -0.5);
//        // 底面
//        glTexCoord2f(1, 1); glVertex3f(-0.5, -0.5, -0.5);
//        glTexCoord2f(0, 1); glVertex3f(0.5, -0.5, -0.5);
//        glTexCoord2f(0, 0); glVertex3f(0.5, -0.5, 0.5);
//        glTexCoord2f(1, 0); glVertex3f(-0.5, -0.5, 0.5);
//        // 左面
//        glTexCoord2f(0, 0); glVertex3f(-0.5, -0.5, -0.5);
//        glTexCoord2f(1, 0); glVertex3f(-0.5, -0.5, 0.5);
//        glTexCoord2f(1, 1); glVertex3f(-0.5, 0.5, 0.5);
//        glTexCoord2f(0, 1); glVertex3f(-0.5, 0.5, -0.5);
//        // 右面
//        glTexCoord2f(0, 0); glVertex3f(0.5, -0.5, 0.5);
//        glTexCoord2f(1, 0); glVertex3f(0.5, -0.5, -0.5);
//        glTexCoord2f(1, 1); glVertex3f(0.5, 0.5, -0.5);
//        glTexCoord2f(0, 1); glVertex3f(0.5, 0.5, 0.5);
//        glEnd();
//
//        glDisable(GL_TEXTURE_2D);
//        glPopMatrix();
//    }
//
//    //-- 引擎喷口（动态环境反射）--
//    glPushMatrix();
//    {
//        transMat1.SetTrans(CVector(0, 0, -0.8f));
//        glMultMatrixf(transMat1);
//
//        glEnable(GL_TEXTURE_CUBE_MAP);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, myShip.envMapTexture);
//        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
//        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
//
//        // 启用立方体贴图坐标生成
//        glEnable(GL_TEXTURE_GEN_S);
//        glEnable(GL_TEXTURE_GEN_T);
//        glEnable(GL_TEXTURE_GEN_R);
//        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
//        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
//        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
//
//        // 动态旋转效果
//        glMatrixMode(GL_TEXTURE);
//        glPushMatrix();
//        glRotatef(float(glutGet(GLUT_ELAPSED_TIME)) / 10.0f, 0, 1, 0);
//        glMatrixMode(GL_MODELVIEW);
//
//        glColor3f(0.8f, 0.8f, 0.8f);
//        glutSolidCone(0.2, 0.4, 16, 8);
//
//        // 恢复矩阵状态
//        glMatrixMode(GL_TEXTURE);
//        glPopMatrix();
//        glMatrixMode(GL_MODELVIEW);
//
//        glDisable(GL_TEXTURE_GEN_S);
//        glDisable(GL_TEXTURE_GEN_T);
//        glDisable(GL_TEXTURE_GEN_R);
//        glDisable(GL_TEXTURE_CUBE_MAP);
//    }
//    glPopMatrix();
//
//
//
//    glPopMatrix(); // 结束主变换
//}