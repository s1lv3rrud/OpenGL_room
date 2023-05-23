#include<gl\glut.h>
#include<gl\glu.h>
#include<gl\gl.h>
#define GL_PI 3.14

int mouseCurButton;
int mouseCurPositionX = 0;
int mouseCurPositionY = 0;
int isDragging = 0;

float distance = 0.0, angle = 0.0, anglexy = 0.0, anglez = 0.0;
float lx = 0.0, ly = 0.0;

int light0Enabled = 1;
int light1Enabled = 0;

void createCylinder(GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat radius, GLfloat h);
void makeStripeImage(void);

static GLuint texName;
static GLuint texName2;
#define stripeImageWidth 32
GLubyte stripeImage[4 * stripeImageWidth];
GLubyte stripeImage2[4 * stripeImageWidth];
static GLfloat xequalzero[] = { 1.0, 0.0, 0.0, 0.0 };
static GLfloat slanted[] = { 1.0, 1.0, 1.0, 0.0 };
static GLfloat* currentCoeff;
static GLenum currentPlane;
static GLint currentGenMode;

void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // 행렬 초기화
    gluPerspective(80.0, 1, 0.1, 5);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);  // 조명 활성화
    glEnable(GL_LIGHT0);    // 0번 광원 활성화
    glEnable(GL_LIGHT1);

    makeStripeImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_1D, texName);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR -> GL_NEAREST
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR -> GL_NEAREST
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD); // GL_MODULATE / GL_BLEND
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, xequalzero);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_1D);
    glEnable(GL_CULL_FACE);
}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat lightPosition0[] = { 0.5f, 0.2f, 0.3f, 0.0f };    // 광원 위치 (x, y, z, w)
    GLfloat lightColor0[] = { 1.0f, 1.0f, 1.0f, 1.0f };        // 광원 색상 (r, g, b, a)
    GLfloat lightambient0[] = { 0.0, 0.0, 0.0, 1.0 }; // 주변광
    GLfloat lightspecular0[] = { 0.0, 0.0, 0.0, 1.0 }; // 반사광

    if (light0Enabled) {
        glEnable(GL_LIGHT0);
    }
    else {
        glDisable(GL_LIGHT0);
    }

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);       // 광원 위치 설정
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);            // 광원 색상 설정
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightambient0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightspecular0);

    GLfloat lightPosition1[] = { 0.2f, 0.5f, 0.3f, 0.0f };    // 광원 위치 (x, y, z, w)
    GLfloat lightColor1[] = { 0.0f, 0.0f, 1.0f, 1.0f };        // 광원 색상 (r, g, b, a)
    GLfloat lightambient1[] = { 0.0, 0.0, 0.0, 1.0 }; // 주변광
    GLfloat lightspecular1[] = { 0.0, 0.0, 0.0, 1.0 }; // 반사광

    if (light1Enabled) {
        glEnable(GL_LIGHT1);
    }
    else {
        glDisable(GL_LIGHT1);
    }

    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);       // 광원 위치 설정
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);            // 광원 색상 설정
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightambient1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightspecular1);

    GLfloat materialSpecular[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat materialAmbient[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat materialShininess[] = { 0.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular); // 반사
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient); // 주변
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);

    gluLookAt(1.3, 1.3, 1.3, 0, 0, 0, 0, 0, 1); // 카메라 위치, 초점, 벡터(축)
    glRotatef(angle, 0, 0, 1); // 회전
    glRotatef(anglexy, 1, 0, 0); // 회전
    glRotatef(-anglexy, 0, 1, 0); // 회전
    glRotatef(anglez, 0, 0, 1); // 회전
    glTranslatef(lx, ly, distance);

    // 바닥
    glPushMatrix();
    GLfloat materialDiffuse[] = { 1, 0.6, 0.6, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glBegin(GL_POLYGON);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    // glFlush();
    glPopMatrix();

    // 오른쪽벽
    glPushMatrix();
    materialDiffuse[0] = 1;
    materialDiffuse[1] = 1;
    materialDiffuse[2] = 0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glBegin(GL_POLYGON);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 1, 1);
    glVertex3f(0, 0, 1);
    glEnd();
    // glFlush();
    glPopMatrix();

    // 왼쪽벽
    glPushMatrix();
    materialDiffuse[0] = 0.8;
    materialDiffuse[1] = 1;
    materialDiffuse[2] = 0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glBegin(GL_POLYGON);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(1, 0, 1);
    glVertex3f(1, 0, 0);
    glEnd();
    // glFlush();
    glPopMatrix();

    // 창문
    glPushMatrix();
    materialDiffuse[0] = 1;
    materialDiffuse[1] = 1;
    materialDiffuse[2] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0, 0.65, 0.6);
    glScalef(1, 16, 14);
    glutSolidCube(-0.03);
    // glFlush();
    glPopMatrix();

    // 선반
    glPushMatrix();
    materialDiffuse[0] = 1.0;
    materialDiffuse[1] = 0.9176;
    materialDiffuse[2] = 0.8235;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.075, 0.15, 0.25);
    glScalef(1.5, 3, 5);
    glutSolidCube(0.1);
    // glFlush();
    glPopMatrix();

    // 소파 밑판
    glPushMatrix();
    materialDiffuse[0] = 0.839;
    materialDiffuse[1] = 0.075;
    materialDiffuse[2] = 0.333;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.55, 0.15, 0.05);
    glScalef(5, 2, 1);
    glutSolidCube(0.1);
    glPopMatrix();

    // 소파 등받이
    glPushMatrix();
    materialDiffuse[0] = 1;
    materialDiffuse[1] = 0.427;
    materialDiffuse[2] = 0.376;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.55, 0.025, 0.125);
    glScalef(5, 0.5, 2.5);
    glutSolidCube(0.1);
    glPopMatrix();

    // 소파 오른쪽
    glPushMatrix();
    materialDiffuse[0] = 0.996;
    materialDiffuse[1] = 0.631;
    materialDiffuse[2] = 0.631;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.825, 0.125, 0.1);
    glScalef(0.5, 2.5, 2);
    glutSolidCube(0.1);
    glPopMatrix();

    // 소파 왼쪽
    glPushMatrix();
    glTranslatef(0.275, 0.125, 0.1);
    glScalef(0.5, 2.5, 2);
    glutSolidCube(0.1);
    glPopMatrix();

    // 책상 상판
    glPushMatrix();
    materialDiffuse[0] = 0.643f;
    materialDiffuse[1] = 0.565f;
    materialDiffuse[2] = 0.486f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.55, 0.4, 0.15);
    glScalef(3.6, 1.5, 0.5);
    glutSolidCube(0.1);
    glPopMatrix();

    // 책상 바깥쪽 다리
    glPushMatrix();
    materialDiffuse[0] = 0.553f;
    materialDiffuse[1] = 0.482f;
    materialDiffuse[2] = 0.408f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.71, 0.4, 0.075);
    glScalef(0.3, 1.3, 1.5);
    glutSolidCube(-0.1);
    glPopMatrix();

    // 책상 안쪽 다리
    glPushMatrix();
    materialDiffuse[0] = 0.553f;
    materialDiffuse[1] = 0.482f;
    materialDiffuse[2] = 0.408f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.39, 0.4, 0.075);
    glScalef(0.3, 1.3, 1.5);
    glutSolidCube(0.1);
    glPopMatrix();

    // 책상 가운데 다리
    glPushMatrix();
    materialDiffuse[0] = 0.553f;
    materialDiffuse[1] = 0.482f;
    materialDiffuse[2] = 0.408f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.55, 0.45, 0.075);
    glScalef(3.6, 0.3, 0.35);
    glutSolidCube(0.1);
    glPopMatrix();

    // 침대 벽
    glPushMatrix();
    materialDiffuse[0] = 0.2235;
    materialDiffuse[1] = 0.2824;
    materialDiffuse[2] = 0.4039;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.025, 0.75, 0.15);
    glScalef(0.5, 4, 3);
    glutSolidCube(0.1);
    glPopMatrix();

    // 침대 밑판
    glPushMatrix();
    materialDiffuse[0] = 0.486;
    materialDiffuse[1] = 0.588;
    materialDiffuse[2] = 0.971;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);  // 재질 설정
    glTranslatef(0.35, 0.75, 0.075);
    glScalef(7, 4, 1.5);
    glBindTexture(GL_TEXTURE_1D, texName);
    glutSolidCube(0.1);
    glPopMatrix();

    // 베개
    glPushMatrix();
    materialDiffuse[0] = 0.5137;
    materialDiffuse[1] = 0.7176;
    materialDiffuse[2] = 0.9804;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
    glTranslatef(0.125, 0.75, 0.175);
    glScalef(1.5, 2, 0.5);
    glutSolidCube(0.1);
    glPopMatrix();

    // 카페트
    glPushMatrix();
    materialDiffuse[0] = 0;
    materialDiffuse[1] = 0.6;
    materialDiffuse[2] = 0.4;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
    glTranslatef(0.85, 0.75, 0);
    glScalef(0.5, 1, 0.5);
    createCylinder(0, 0, 0, 0.2, 0.05);
    glPopMatrix();

    glutSwapBuffers();

    glTranslatef(0, -0.5, -0.8);
}

void mouseStatus(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_RIGHT_BUTTON) {
            if (x <= 250) {
                // z축 기준 시계
                angle -= 10;
            }
            else {
                // z축 기준 시계반대
                angle += 10;
            }
            mouseCurButton = button;
            mouseCurPositionX = x;
            mouseCurPositionY = y;
        }
        else {
            if (state == GLUT_DOWN) {
                mouseCurPositionX = x;
                mouseCurPositionY = y;
                isDragging = 1;
            }
            else if (state == GLUT_UP) {
                isDragging = 0;
            }
        }
    }

    glutPostRedisplay();

}

void mouseMotion(int x, int y) {
    if (isDragging) {
        anglexy += (mouseCurPositionY - y) * 0.2f;
        anglez += (x - mouseCurPositionX) * 0.2f;
        mouseCurPositionX = x;
        mouseCurPositionY = y;
        glutPostRedisplay();
    }
}

void keyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
    case '0':
        light0Enabled = 1; // Toggle
        light1Enabled = 0; // Toggle
        break;
    case '1':
        light1Enabled = 1; // Toggle
        light0Enabled = 0; // Toggle
        break;
    case '2':
        light0Enabled = 1; // Enable light0
        light1Enabled = 1; // Enable light1
        break;
    case '3':
        light0Enabled = 0; // Disable light0
        light1Enabled = 0; // Disable light1
        break;
    }

    glutPostRedisplay();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("21011912 이은경");

    init();

    glutDisplayFunc(myDisplay);

    glutMouseFunc(mouseStatus);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboardFunc);

    glutMainLoop();
}

void createCylinder(GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat radius, GLfloat h)
{
    /* function createCyliner()
    원기둥의 중심 x,y,z좌표, 반지름, 높이를 받아 원기둥을 생성하는 함수(+z방향으로 원에서 늘어남)
    centerx : 원기둥 원의 중심 x좌표
    centery : 원기둥 원의 중심 y좌표
    centerz : 원기둥 원의 중심 z좌표
    radius : 원기둥의 반지름
    h : 원기둥의 높이
    */
    GLfloat x, y, angle;

    glBegin(GL_TRIANGLE_FAN);           //원기둥의 윗면
    glNormal3f(0.0f, 0.0f, -1.0f);
    glColor3ub(139, 69, 19);
    glVertex3f(centerx, centery, centerz);


    for (angle = 0.0f; angle < (2.0f * GL_PI); angle += (GL_PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(x, y, centerz);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);            //원기둥의 옆면
    for (angle = 0.0f; angle < (2.0f * GL_PI); angle += (GL_PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(sin(angle), cos(angle), 0.0f);
        glVertex3f(x, y, centerz);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);           //원기둥의 밑면
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(centerx, centery, centerz + h);
    for (angle = (2.0f * GL_PI); angle > 0.0f; angle -= (GL_PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();
}

void makeStripeImage(void)
{
    int j;
    for (j = 0; j < stripeImageWidth; j++) {
        stripeImage[4 * j] = (GLubyte)((j <= 4) ? 255 : 0);
        stripeImage[4 * j + 1] = (GLubyte)((j > 4) ? 255 : 0);
        stripeImage[4 * j + 2] = (GLubyte)0;
        stripeImage[4 * j + 3] = (GLubyte)255;
    }
}