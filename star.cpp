#include "CVector.h" 
#include "star.h" 
#include "base.h"

#define S_NUM 352//星星数量

star s_l[S_NUM] = {};//star_list struct star类型的列表

void initstar() {
    for (int i = 0; i < S_NUM; i++) {
        s_l[i].place.x = random_float(-6.0f, 6.0f);
        s_l[i].place.y = random_float(-6.0f, 6.0f);
        s_l[i].place.z = random_float(-6.0f, 6.0f);
        s_l[i].color.z = random_float(0.0f, 5.0f);
        s_l[i].size = random_float(2.5f, 4.0f);
        s_l[i].speed.x = random_float(-0.01, 0.01);
        s_l[i].speed.y = random_float(-0.01, 0.01);
        s_l[i].speed.z = random_float(-0.01, 0.01);
        s_l[i].colorDelta = random_float(0.02, 0.07);
    }
}

void drawStars() {
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < S_NUM; i++) {
        glPointSize(s_l[i].size);
        glBegin(GL_POINTS);
        float temp = sin(s_l[i].color.z);
        float s_color = temp > 0 ? temp : -temp;
        glColor3f(s_color, s_color, s_color);
        glVertex3fv(s_l[i].place);
        glEnd();
    }
}

void checkStar() {
    const float boundary = 6.0f; // 星星边界范围
    for (int i = 0; i < S_NUM; i++) {
        s_l[i].color.z = s_l[i].color.z + s_l[i].colorDelta;
        s_l[i].place = s_l[i].place + s_l[i].speed;
        for (int axis = 0; axis < 3; axis++) {// 检查每个坐标轴是否越界并反弹
            if (s_l[i].place[axis] > boundary) {
                s_l[i].speed[axis] *= -1.0f;//速度反向
                s_l[i].place[axis] = boundary;
            }
            else if (s_l[i].place[axis] < -boundary) {
                s_l[i].speed[axis] *= -1.0f;
                s_l[i].place[axis] = -boundary;
            }
        }
    }
}