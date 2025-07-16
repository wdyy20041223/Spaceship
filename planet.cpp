#include "CVector.h" 
#include "planet.h" 
#include "base.h"
#include "ship.h"
#include "global.h"
#include "CMatrix.h" 



#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define A2R(x) (x/180.0*PI) //�Ƕ�ת����

bool g_wireframe = false;
ball planet[8];//struct ball����
float radiusCorrection = 1.2, distanceCorrection = 1.1, rotationCorrection = 1.1;//һЩ����������ϵ��
ball* selectedPlanet = nullptr; // ��ǰѡ�е�����
extern Camera planetCamera;
extern ball planet[8];

GLuint LoadTexture(const char* path) {
    int width, height, channels;

    // ǿ�Ƽ���Ϊ4ͨ��������PNG��Alphaͨ����
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
    if (!data || width <= 0 || height <= 0) {
        printf("��������ʧ��: %s\n", path);
        return 0;
    }

    // ͳһת��ΪRGBA��ʽ
    GLenum format = GL_RGBA;
    GLint internalFormat = GL_RGBA8;  // �����ڲ���ʽ

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // �ؼ��޸�1������1�ֽڶ���
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // �ؼ��޸�2��ʹ����ȷ���ڲ���ʽ
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
        width, height, 0, format,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // ͳһ���û�ϲ����������Ƿ���Alphaͨ����
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // �ӳ��ͷ�����ֱ��ȷ�ϳɹ�
    stbi_image_free(data);

    return textureID;
}

void drawPlanet() {

    for (int i = 0; i < 8; i++) {
        drawTrack(planet[i]);
        drawBall(planet[i]);
        if (planet[i].selected) {//����ѡ��Ч��
            glPushMatrix();
            transMat1.SetTrans(CVector(planet[i].centerPlace.x, planet[i].centerPlace.y, planet[i].centerPlace.z));
            glMultMatrixf(transMat1);
            glColor3f(1,1,1); // �߿�
            glutWireSphere(planet[i].r * 1.15, 12, 12);
            glPopMatrix();

            planetCamera.origonPos = planet[i].centerPlace + CVector(0, 1, 0) * planet[i].r * 1.2;       
            CVector dir = -planetCamera.origonPos;
            planetCamera.realOrientation = (-dir).ToEuler().ToQuaternion();
            planetCamera.position = planetCamera.origonPos;


        }
    }
    drawRing(planet[6]);
}

void mouseClick(int button, int state, int x, int y) {
    bool successTag = false;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (selectedPlanet != nullptr) {// ���֮ǰѡ�е�����
            selectedPlanet->selected = false;
            selectedPlanet = nullptr;
        }
        // ת��������굽3D����
        GLint viewport[4];
        GLdouble modelview[16], projection[16];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);

        GLdouble startX, startY, startZ;
        gluUnProject(x, viewport[3] - y, 0, modelview, projection, viewport, &startX, &startY, &startZ);

        GLdouble endX, endY, endZ;
        gluUnProject(x, viewport[3] - y, 1, modelview, projection, viewport, &endX, &endY, &endZ);
        // ������������ײ���
        CVector rayDir(endX - startX, endY - startY, endZ - startZ);
        rayDir.Normalize();

        for (int i = 0; i < 8;i++) { // ��������
            CVector toCenter = planet[i].centerPlace - CVector(startX, startY, startZ);
            float t = toCenter.dotMul(rayDir);
            if (t < 0) continue; // ���߷���

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
    strcpy_s(planet[0].name, "sun");
    planet[0].textureID = LoadTexture("textures/sun.jpg");
    strcpy_s(planet[1].name, "mercury");
    planet[1].textureID = LoadTexture("textures/mercury.jpg");
    strcpy_s(planet[2].name, "venus");
    planet[2].textureID = LoadTexture("textures/venus.jpg");
    strcpy_s(planet[3].name, "earth");
    planet[3].textureID = LoadTexture("textures/earth.jpg");
    strcpy_s(planet[4].name, "mars");
    planet[4].textureID = LoadTexture("textures/mars.jpg");
    strcpy_s(planet[5].name, "jupiter");
    planet[5].textureID = LoadTexture("textures/jupiter.jpg");
    strcpy_s(planet[6].name, "saturn");
    planet[6].textureID = LoadTexture("textures/saturn.jpg");
    planet[6].ringTextureID = LoadTexture("textures/saturn_ring.png");
    strcpy_s(planet[7].name, "moon");
    planet[7].textureID = LoadTexture("textures/moon.jpg");

    for (int i = 0; i < 8; i++) {
        planet[i].index = i;
    }

    planet[0].color = CVector(1.0f, 0.0f, 0.0f); //̫��
    planet[0].r = 0.6 * radiusCorrection;
    planet[0].rotationSpeed = 0.065 * rotationCorrection*10;
    initBall(planet[0]);

    planet[1].color = CVector(0.0f, 0.0f, 1.0f); //ˮ��
    planet[1].r = 0.1 * radiusCorrection;
    planet[1].rotationSpeed = 0.01 * 40 * rotationCorrection;
    planet[1].orbitSpeed = 0.36 * 4;
    planet[1].orbitRadius = 1.0f * distanceCorrection;
    planet[1].centerPlace = CVector(planet[1].orbitRadius, 0.0f, 0.0f);
    planet[1].orbitCenter = planet[0].centerPlace;
    planet[1].pointNum = 20;
    planet[1].startNum = 5;
    initBall(planet[1]);

    planet[2].color = CVector(1.0f, 1.0f, 0.0f); //����
    planet[2].r = 0.18 * radiusCorrection;
    planet[2].rotationSpeed = 0.018 * 40 * rotationCorrection;
    planet[2].orbitSpeed = 0.36 * 1.5;
    planet[2].orbitRadius = 1.5f * distanceCorrection;
    planet[2].centerPlace = CVector(planet[2].orbitRadius, 0.0f, 0.0f);
    planet[2].orbitCenter = planet[0].centerPlace;
    planet[2].pointNum = 35;
    planet[2].startNum = 10;
    initBall(planet[2]);

    planet[3].color = CVector(0.0f, 1.0f, 1.0f); //����
    planet[3].r = 0.2 * radiusCorrection;
    planet[3].rotationSpeed = 0.02 * 40 * rotationCorrection;
    planet[3].orbitSpeed = 0.36;
    planet[3].orbitRadius = 2.0f * distanceCorrection;
    planet[3].centerPlace = CVector(planet[3].orbitRadius, 0.0f, 0.0f);
    planet[3].orbitCenter = planet[0].centerPlace;
    planet[3].pointNum = 45;
    planet[3].startNum = 15;
    initBall(planet[3]);

    planet[4].color = CVector(0.6f, 0.2f, 0.2f); //����
    planet[4].r = 0.16 * radiusCorrection * rotationCorrection;
    planet[4].rotationSpeed = 0.016 * 40;
    planet[4].orbitSpeed = 0.36 / 2;
    planet[4].orbitRadius = 2.5f * distanceCorrection;
    planet[4].centerPlace = CVector(planet[4].orbitRadius, 0.0f, 0.0f);
    planet[4].orbitCenter = planet[0].centerPlace;
    planet[4].pointNum = 70;
    planet[4].startNum = 20;
    initBall(planet[4]);

    planet[5].color = CVector(0.6f, 0.6f, 0.2f); //ľ��
    planet[5].r = 0.37 * radiusCorrection * rotationCorrection;
    planet[5].rotationSpeed = 0.037 * 15;
    planet[5].orbitSpeed = 0.36 / 11 * 2;//����
    planet[5].orbitRadius = 3.1f * distanceCorrection;
    planet[5].centerPlace = CVector(planet[5].orbitRadius, 0.0f, 0.0f);
    planet[5].orbitCenter = planet[0].centerPlace;
    planet[5].pointNum = 280;
    planet[5].startNum = 100;
    initBall(planet[5]);

    planet[6].color = CVector(0.6f, 0.9f, 0.3f); //����
    planet[6].r = 0.30 * radiusCorrection * rotationCorrection;
    planet[6].rotationSpeed = 0.030 * 40;
    planet[6].orbitSpeed = 0.36 / 30 * 3;//����
    planet[6].orbitRadius = 4.2f * distanceCorrection;
    planet[6].centerPlace = CVector(planet[6].orbitRadius, 0.0f, 0.0f);
    planet[6].orbitCenter = planet[0].centerPlace;
    planet[6].pointNum = 350;
    planet[6].startNum = 120;
    initBall(planet[6]);

    planet[7].color = CVector(0.6, 0.6, 0.6); //����
    planet[7].r = 0.05 * radiusCorrection * rotationCorrection;
    planet[7].rotationSpeed = 0.005 * 40;
    planet[7].orbitSpeed = 0.36 * 4;
    planet[7].orbitRadius = 0.3f * distanceCorrection;
    planet[7].centerPlace = CVector(planet[7].orbitRadius + planet[3].orbitRadius, 0.0f, 0.0f);//����
    planet[7].orbitCenter = planet[3].centerPlace;//�Ƶ���
    planet[7].pointNum = 25;
    strcpy_s(planet[7].name, "moon");
    initBall(planet[7]);



}

void drawRing(ball saturn) {
    glPushMatrix();
    transMat1.SetTrans(CVector(saturn.orbitPoints[saturn.index].x,
        saturn.orbitPoints[saturn.index].y,
        saturn.orbitPoints[saturn.index].z));
    rotateMat1.SetRotate(33.5, CVector(0, 0, 1));
    rotateMat2.SetRotate(saturn.rotationAngle, CVector(0, 1, 0));
    glMultMatrixf(transMat1 * rotateMat1 * rotateMat2);

    // ���û�Ϻ���Ȳ���
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    // �󶨻�����
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, saturn.ringTextureID);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // �����β���
    const float inner = 1.6f * saturn.r;
    const float outer = 2.1f * saturn.r;
    const int slices = 72;    // ����ֶ�
    const int stacks = 32;    // ����ֶ�

    // ���ɻ�״�ı�������
    for (int i = 0; i < slices; ++i) {
        float theta1 = 2.0f * PI * i / slices;
        float theta2 = 2.0f * PI * (i + 1) / slices;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= stacks; ++j) {
            float ratio = (float)j / stacks;
            float radius = inner + (outer - inner) * ratio;

            // ��������ӳ��
            float s = (float)i / slices;
            float t = ratio;

            // ���㶥��λ��
            CVector v1 = CVector(radius * cos(theta1), 0, radius * sin(theta1));
            CVector v2 = CVector(radius * cos(theta2), 0, radius * sin(theta2));

            glTexCoord2f(s, t);
            glVertex3fv(v1);

            glTexCoord2f(s + 1.0f / slices, t);
            glVertex3fv(v2);
        }
        glEnd();
    }

    // �ָ�״̬
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glPopMatrix();
}

void initBall(ball& ball0) {
    int temp1 = 4;
    int temp2 = 6;
    for (int i = -90; i <= 90; i += temp1)
    {
        for (int j = 0; j <= 360; j += temp2)
        {
            float r = ball0.r * cos(A2R(i));
            ball0.pointPlace[(90 + i) / temp1][j / temp2].x = r * sin(A2R(j));
            ball0.pointPlace[(90 + i) / temp1][j / temp2].y = r * cos(A2R(j));
            ball0.pointPlace[(90 + i) / temp1][j / temp2].z = ball0.r * sin(A2R(i));

            float u = (j) / 360.0f;
            float v = (i + 90.0f) / 180.0f;
            ball0.texCoords[(90 + i) / temp1][j / temp2] = CVector2(u, v);

            // ���㷨�ߣ���ӵ�����ѭ���ڣ�
            ball0.normalVectors[(90 + i) / temp1][j / temp2] = CVector(
                ball0.pointPlace[(90 + i) / temp1][j / temp2].x ,
                ball0.pointPlace[(90 + i) / temp1][j / temp2].y,
                ball0.pointPlace[(90 + i) / temp1][j / temp2].z 
            ).Normalized();
        }
    }
    ball0.pointPlace[60][60] = ball0.pointPlace[0][0]; 

    for (int j = 0; j < 361; j++) {//��ʼ���켣
        ball0.orbitPoints[j] = ball0.centerPlace;
    }
}

void drawBall(ball& ball0) {
    glPushMatrix();
    transMat1.SetTrans(CVector(ball0.orbitPoints[ball0.index].x,
        ball0.orbitPoints[ball0.index].y,
        ball0.orbitPoints[ball0.index].z));
    rotateMat1.SetRotate(33.5, CVector(0, 0, 1));
    rotateMat2.SetRotate(ball0.rotationAngle, CVector(0, 1, 0));
    CMatrix finalMat = transMat1 * rotateMat1 * rotateMat2;
    glMultMatrixf(finalMat);

    ball0.r = ball0.r * 2;
    ball0.box.partName = ball0.name;
    ball0.box.min = CVector(-ball0.r / 2, -ball0.r / 2, -ball0.r / 2); 
    ball0.box.max = CVector(ball0.r / 2, ball0.r / 2, ball0.r / 2);
    ball0.box.worldTransform = finalMat;
    ball0.r = ball0.r / 2;

    // �������������
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ball0.textureID);


    glEnable(GL_LIGHTING); // ȷ������
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); // ȷ�����÷ɴ�ǰ��

    // ���ò��ʺ�������
    if (strcmp(ball0.name, "sun") == 0) {
        // ��ǿ̫���Է���
        GLfloat emission[] = { 5.0f, 4.5f, 4.0f, 1.0f };  // ԭΪ3.0,2.5,2.0
        // ��������������
        GLfloat diffuse[] = { 0.4f, 0.35f, 0.3f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    }
    else {

        // ��ǿ���ǲ��ʷ���
        GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // ���ͻ�����
        GLfloat diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };  // ���ָ�������
        GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ��󻯾��淴��
        GLfloat shininess = 128.0f;                   // ���ӹ����

        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }

    // �����ı��δ�����ӷ���
    for (int i = 0; i < 60; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j < 61; j++) {
            glTexCoord2fv(ball0.texCoords[i][j]);
            glNormal3fv(ball0.normalVectors[i][j]);
            glVertex3fv(ball0.pointPlace[i][j]);

            glTexCoord2fv(ball0.texCoords[i + 1][j]);
            glNormal3fv(ball0.normalVectors[i + 1][j]);
            glVertex3fv(ball0.pointPlace[i + 1][j]);
        }
        glEnd();
    }
    
    // �ָ�Ĭ������
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    GLfloat default_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, default_emission);

    // ������ɫ״̬����Ҫ����
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();

}

void planetRotation() {//��ת
    for (int i = 0; i < 8; i++) {
        planet[i].rotationAngle += planet[i].rotationSpeed;
        planet[i].orbitAngle += planet[i].orbitSpeed;
        if (planet[i].orbitAngle > 360)
            planet[i].orbitAngle -= 360;

        planet[i].index = (planet[i].index + 1) % 360;

        float radian = A2R(planet[i].orbitAngle); // ���㵱ǰ�������꣨���ļ��㹫ʽ��
        planet[i].orbitPoints[planet[i].index] =
            planet[i].orbitCenter + CVector(planet[i].orbitRadius * cos(radian), 0, -planet[i].orbitRadius * sin(radian));
        planet[i].centerPlace = planet[i].orbitPoints[planet[i].index];

        if (i == 3) {
            planet[7].orbitCenter = planet[3].orbitPoints[planet[3].index];// ����ת����
        }
    }
}

void drawTrack(ball ball0) {

    // ����ԭ�е���ʷ�켣���ƣ����ߣ�
    glPushMatrix();
    glColor3f(1, 1, 1);
    glLineWidth(3.5f);          // ��ʷ�켣���ִ���
    glBegin(GL_LINE_STRIP);
    int currentIndex = ball0.index;
    for (int i = ball0.startNum; i < ball0.pointNum; i++) {
        int idx = (currentIndex - i + 360) % 360;
        glVertex3fv(ball0.orbitPoints[idx]);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();// ��������Բ�ι켣��ϸ�ߣ�
    glColor3f(0.5f, 0.5f, 1.0f); // ���õ���ɫ����
    glLineWidth(1.5f);           // ϸ������

    transMat1.SetTrans(CVector(ball0.orbitCenter.x, ball0.orbitCenter.y, ball0.orbitCenter.z));
    glMultMatrixf(transMat1);
    const int segments = 90;    // ���ٷֶ�����������
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
