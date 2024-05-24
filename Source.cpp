
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <gl/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int cw;
int ch;
int cTurn = 180; //current facing

bool isStand = true;
float curDistanceX = 0, curDistanceZ = 0;//current dist
float legDis = 0;

void timerCallback(int value); // Declaration of the timer callback function
GLfloat headRotationAngle = 90.0;
GLfloat armRotationAngle = 0.0;
GLfloat forearmRotationAngle = 0.0;

enum CameraView {
    FIRST_PERSON,
    THIRD_PERSON,
    TOP_VIEW
};
CameraView currentView = THIRD_PERSON; // Initial camera view

float cameraAngleX = 0.0f; // Angle to rotate the camera around the x-axis
float cameraAngleY = 0.0f; // Angle to rotate the camera around the y-axis
float cameraPosX = 0.0f; // Camera position X
float cameraPosY = 1.6f; // Camera position Y (head height)
float cameraPosZ = 0.0f; // Camera position Z
float cameraSpeed = 0.1f; // Speed of camera movement

float characterPosX = 0.0f; // Character position X
float characterPosZ = 0.0f; // Character position Z
float characterPosY = 0.0f; // Character position Z

int lastMouseX = 0;    // Last known X position of the mouse
int lastMouseY = 0;    // Last known Y position of the mouse
bool isDragging = false;  // Flag to check if mouse is being dragged
int currentCharacter = 1;
bool isJumping = false;  // Flag to check if the character is jumping
float jumpVelocity = 0.0f; // Initial jump velocity
float gravity = 0.0005f;   // Gravity acceleration
float jumpSpeed = 0.05f;  // Speed of the jump

bool isMovingUp = false;  // Flag to check if the character is moving up
bool isMovingDown = false; // Flag to check if the character is moving down
float originalY = 0.0f;
bool isWalking = false;
float walkingLegAngle = 0.0f;
float walkingLegSpeed = 0.1f;

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        currentView = FIRST_PERSON;
        break;
    case '2':
        currentView = THIRD_PERSON;
        break;
    case '3':
        currentView = TOP_VIEW;
        break;

    case 'p':
        currentCharacter = 1;
        break;
    case 'o':
        currentCharacter = 2;
        break;
    case'i':
        currentCharacter = 3;
        break;
    case 'w':
        characterPosX += sin(cameraAngleY * M_PI / 180) * cameraSpeed * 10;
        characterPosZ += cos(cameraAngleY * M_PI / 180) * cameraSpeed * 10;
        isWalking = true;
        break;
    case 's':
        characterPosX -= sin(cameraAngleY * M_PI / 180) * cameraSpeed * 10;
        characterPosZ -= cos(cameraAngleY * M_PI / 180) * cameraSpeed * 10;
        isWalking = true;
        break;
    case 'a':
        characterPosX += cos(cameraAngleY * M_PI / 180) * cameraSpeed * 10;
        characterPosZ -= sin(cameraAngleY * M_PI / 180) * cameraSpeed * 10;
        isWalking = true;
        break;
    case 'd':
        characterPosX -= cos(cameraAngleY * M_PI / 180) * cameraSpeed * 10;
        characterPosZ += sin(cameraAngleY * M_PI / 180) * cameraSpeed * 10;
        isWalking = true;
        break;
    case 'j': // Start jumping
        if (!isJumping) {
            isJumping = true;
            jumpVelocity = jumpSpeed; // Initial upward velocity
            glutTimerFunc(5000, timerCallback, 0);
        }
        break;
    default:
        isWalking = false;
        break;
    }

    // Redraw the scene
    glutPostRedisplay();
}
void specialKeys(int keys, int x, int y) {
    float speed = 0.1;
    switch (keys) {
    case GLUT_KEY_UP:

        characterPosX += sin(cameraAngleY * M_PI / 180) * cameraSpeed;
        characterPosZ += cos(cameraAngleY * M_PI / 180) * cameraSpeed;
        isWalking = true;

        break;

    case GLUT_KEY_DOWN:
        characterPosX -= sin(cameraAngleY * M_PI / 180) * cameraSpeed;
        characterPosZ -= cos(cameraAngleY * M_PI / 180) * cameraSpeed;
        isWalking = true;
        break;

    case GLUT_KEY_LEFT:
        characterPosX += cos(cameraAngleY * M_PI / 180) * cameraSpeed;
        characterPosZ -= sin(cameraAngleY * M_PI / 180) * cameraSpeed;
        isWalking = true;
        break;
    case GLUT_KEY_RIGHT:
        characterPosX -= cos(cameraAngleY * M_PI / 180) * cameraSpeed;
        characterPosZ += sin(cameraAngleY * M_PI / 180) * cameraSpeed;
        isWalking = true;
        break;
    default:
        isWalking = false;
        break;
    }

    glutPostRedisplay();
}
void specialKeysUp(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        isWalking = false;
        break;
    case GLUT_KEY_DOWN:
        isWalking = false;
        break;
    case GLUT_KEY_LEFT:
        isWalking = false;
        break;
    case GLUT_KEY_RIGHT:
        isWalking = false;
        break;
    default:
        break;
    }
}

GLfloat vertices[8][3] = {
  {-0.5, -0.5, 0.5},
  {0.5, -0.5, 0.5},
  {0.5, 0.5, 0.5},
  {-0.5, 0.5, 0.5},
  {-0.5, -0.5, -0.5},
  {0.5, -0.5, -0.5},
  {0.5, 0.5, -0.5},
  {-0.5, 0.5, -0.5}
};
GLfloat vertice1[8][3] = {
  {-0.6, -0.3, 0.6},
  {0.6, -0.3, 0.6},
  {0.6, 0.3, 0.6},
  {-0.6, 0.3, 0.6},
  {-0.6, -0.3, -0.6},
  {0.6, -0.3, -0.6},
  {0.6, 0.3, -0.6},
  {-0.6, 0.3, -0.6}
};

void enable() {
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

void cylinder(int segments, float radius, float height) {
    int k;
    float angles;
    float x, z;

    glPushMatrix();
    glTranslatef(0.0, height / 2, 0.0);
    glBegin(GL_POLYGON);
    for (k = 0; k < segments; k++) {
        angles = 2.0 * 3.141592654f * k / segments;
        x = radius * cos(angles);
        z = radius * sin(angles);
        glVertex3f(x, 0, z);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, -height / 2, 0.0);
    glBegin(GL_POLYGON);
    for (k = 0; k < segments; k++) {
        angles = 2.0 * 3.141592654f * k / segments;
        x = radius * cos(angles);
        z = radius * sin(angles);
        glVertex3f(x, 0, z);
    }
    glEnd();
    glPopMatrix();


    glBegin(GL_QUAD_STRIP);
    for (k = 0; k <= segments; k++) {
        angles = 2.0 * 3.141592654f * k / segments;
        x = radius * cos(angles);
        z = radius * sin(angles);
        glVertex3f(x, -height / 2, z);
        glVertex3f(x, height / 2, z);
    }
    glEnd();
}

void polygon(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
    glVertex3fv(vertices[a]);
    glVertex3fv(vertices[b]);
    glVertex3fv(vertices[c]);
    glVertex3fv(vertices[d]);
    glEnd();
}

void polygon1(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
    glVertex3fv(vertice1[a]);
    glVertex3fv(vertice1[b]);
    glVertex3fv(vertice1[c]);
    glVertex3fv(vertice1[d]);
    glEnd();
}

void cube()
{
    polygon(0, 1, 2, 3);
    polygon(4, 5, 6, 7);
    polygon(0, 4, 7, 3);
    polygon(1, 5, 6, 2);
    polygon(3, 2, 6, 7);
    polygon(0, 1, 5, 4);
}

void cube1()
{
    polygon1(0, 1, 2, 3);
    polygon1(4, 5, 6, 7);
    polygon1(0, 4, 7, 3);
    polygon1(1, 5, 6, 2);
    polygon1(3, 2, 6, 7);
    polygon1(0, 1, 5, 4);
}

void square()
{
    glBegin(GL_POLYGON);
    glVertex3f(-0.3, 0.0, 0.5); // bottom left
    glVertex3f(-0.1, 0, 0.5); // bottom right
    glVertex3f(-0.1, 0.2, 0.5); // top right
    glVertex3f(-0.3, 0.2, 0.5); // top left
    glEnd();
}

void mouth()
{
    glBegin(GL_POLYGON);
    glVertex3f(-0.35, -0.1, 0.5); // bottom left
    glVertex3f(-0.2, -0.25, 0.5); // bottom right
    glVertex3f(0.1, -0.25, 0.5); // top right
    glVertex3f(0.25, -0.1, 0.5); // top left
    glVertex3f(0.2, -0.1, 0.5); // bottom left
    glVertex3f(0.1, -0.2, 0.5); // bottom right
    glVertex3f(-0.2, -0.2, 0.5); // bottom left
    glVertex3f(-0.3, -0.1, 0.5); // bottom right

    glEnd();
}

void drawRobloxCharacter() {

    glPushMatrix();
    glScalef(0.5, 0.5, 0.5);

    glPushMatrix();

    glRotatef((GLfloat)cTurn, 0.0, 1.0, 0.0); //turn

    glTranslatef(0.375, 0.0, 0.0);

    //rl
    glTranslatef(0.0, 0.7, 0.0);
    glColor3f(0.5, 0.8, 0.3);
    //rl
    glPushMatrix();
    glScalef(0.5, 0.8, 0.5);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();

    //ll
    glPushMatrix();
    glRotatef((GLfloat)cTurn, 0.0, 1.0, 0.0); //turn

    //ll
    glTranslatef(-0.375, 0.0, 0.0);
    glTranslatef(0.0, 0.7, 0.0);
    glColor3f(0.5, 0.8, 0.3);
    glPushMatrix();
    glScalef(0.5, 0.8, 0.5);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();


    glPushMatrix();
    glTranslatef(curDistanceX, 0.0, curDistanceZ);
    glRotatef((GLfloat)cTurn, 0.0, 1.0, 0.0);
    //body
    glTranslatef(0.0, 2.0, 0.0);
    glColor3f(0.0, 0.0, 0.5);
    glPushMatrix();
    glScalef(1.4, 1.75, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();
    //head
    glTranslatef(0.0, 1.55, 0.0);
    glColor3f(1.0, 0.75, 0.0);
    glPushMatrix();
    glScalef(0.8, 0.8, 0.8);
    glutSolidSphere(0.75, 20, 20);
    glPopMatrix();

    //eyes
    glTranslatef(0.125, 0.1, -0.6);
    glColor3f(0.2, 0.2, 0.2);
    glPushMatrix();
    glScalef(0.1, 0.1, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    glTranslatef(-0.25, 0.0, 0.0);
    glColor3f(0.2, 0.2, 0.2);
    glPushMatrix();
    glScalef(0.1, 0.1, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    //mouth
    glTranslatef(0.125, -0.25, 0.02);
    glColor3f(0.8, 0.2, 0.2);
    glPushMatrix();
    glScalef(0.3, 0.05, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();

    //ra
    glPushMatrix();
    glTranslatef(curDistanceX, 0.0, curDistanceZ);
    glRotatef((GLfloat)cTurn, 0.0, 1.0, 0.0);
    //ra
    glTranslatef(0.85, 1.80, 0.0);

    glTranslatef(0.0, 0.5, 0.0);
    glColor3f(1.0, 0.75, 0.0);
    glPushMatrix();
    glScalef(0.35, 1.0, 0.45);
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();

    //la
    glPushMatrix();
    glTranslatef(curDistanceX, 0.0, curDistanceZ);
    glRotatef((GLfloat)cTurn, 0.0, 1.0, 0.0);
    //la
    glTranslatef(-0.85, 1.8, 0.0);

    glTranslatef(0.0, 0.5, 0.0);
    glColor3f(1.0, 0.75, 0.0);
    glPushMatrix();
    glScalef(0.35, 1.0, 0.45);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();

    //legs
    glPushMatrix();
    glTranslatef(curDistanceX, 0.0, curDistanceZ);
    glRotatef((GLfloat)cTurn, 0.0, 1.0, 0.0);
    //rl
    glTranslatef(0.375, 0.0, 0.0);
    glTranslatef(0.0, 0.7, 0.0);
    glColor3f(0.5, 0.8, 0.3);
    //rl
    glPushMatrix();
    glScalef(0.5, 0.8, 0.5);
    if (isWalking) {
        // Adjust leg angle for walking animation
        glRotatef(walkingLegAngle, 1.0, 0.0, 0.0);
    }
    glutSolidCube(1.0);
    glPopMatrix();
    //ll
    glTranslatef(-0.75, 0.0, 0.0);
    glTranslatef(0.0, 0.7, 0.0);
    glColor3f(0.5, 0.8, 0.3);
    glPushMatrix();
    glScalef(0.5, 0.8, 0.5);
    if (isWalking) {
        // Adjust leg angle for walking animation
        glRotatef(-walkingLegAngle, 1.0, 0.0, 0.0);
    }
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
}

void drawCharacter2() {
    drawRobloxCharacter();
}

void drawCharacter1() {

    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0); // Yellow
    glTranslatef(0.0, 1.0, 0.10); // Adjust position as needed
    glScalef(0.1, 0.1, 0.1); // Scale to make it elliptical if needed
    cylinder(24, 1.0, 2.0); // Draw cylinder with given segments, radius and height
    glPopMatrix();

    glPushMatrix(); //body
    glTranslatef(0, 0.6, 0.1);
    glScalef(0.45, 0.45, 0.45);
    glColor3f(0.0, 0.0, 1.0); // blue
    cube();
    glPopMatrix();

    glPushMatrix();//right hand 1
    glTranslatef(0.3, 0.6, 0.1);
    glScalef(0.17, 0.17, 0.17);
    glColor3f(1.0, 1.0, 0.0); // yellow
    cube();
    glPopMatrix();

    glPushMatrix();//right hand 2
    glTranslatef(0.3, 0.45, 0.125);
    glScalef(0.17, 0.17, 0.17);
    glColor3f(1.0, 1.0, 0.0); // yellow
    glRotatef(22.5, -1.0, 0.0, 0.0);
    cube();
    glPopMatrix();

    glPushMatrix();//left hand 1
    glTranslatef(-0.3, 0.6, 0.1);
    glScalef(0.17, 0.17, 0.17);
    glColor3f(1.0, 1.0, 0.0); // yellow
    cube();
    glPopMatrix();

    glPushMatrix();//left hand 2
    glTranslatef(-0.3, 0.45, 0.125);
    glScalef(0.17, 0.17, 0.17);
    glColor3f(1.0, 1.0, 0.0); // yellow
    glRotatef(22.5, -1.0, 0.0, 0.0);
    cube();
    glPopMatrix();

    glPushMatrix();//right leg 1
    glTranslatef(0.14, 0.3, 0.04);
    glScalef(0.17, 0.17, 0.17);
    glColor3f(0.0f, 0.5f, 0.5f); // green

    cube();
    glPopMatrix();

    glPushMatrix();//right leg 3
    glTranslatef(0.14, 0.15, 0.04);
    glScalef(0.17, 0.17, 0.17);
    glColor3f(0.0f, 0.5f, 0.5f); // green
    cube();
    glPopMatrix();

    glPushMatrix();//right leg 2
    glTranslatef(0.14, 0.00, 0.05);
    glScalef(0.19, 0.19, 0.19);
    glColor3f(0.5f, 1.0f, 1.0f); // green
    if (isWalking) {
        // Adjust leg angle for walking animation
        glRotatef(walkingLegAngle, 1.0, 0.0, 0.0);
    }
    cube1();
    glPopMatrix();

    glPushMatrix();//left leg 1
    glTranslatef(-0.14, 0.3, 0.04);
    glScalef(0.17, 0.17, 0.17);
    glColor3f(0.0f, 0.5f, 0.5f); // green
    cube();
    glPopMatrix();

    glPushMatrix();//left leg 3
    glTranslatef(-0.14, 0.15, 0.04);
    glScalef(0.17, 0.17, 0.17);
    glColor3f(0.0f, 0.5f, 0.5f); // green
    cube();
    glPopMatrix();

    glPushMatrix();//left leg 2
    glTranslatef(-0.14, 0.0, 0.05);
    glScalef(0.19, 0.19, 0.19);
    glColor3f(0.5f, 1.0f, 1.0f); // green
    if (isWalking) {
        // Adjust leg angle for walking animation
        glRotatef(-walkingLegAngle, 1.0, 0.0, 0.0);
    }
    cube1();
    glPopMatrix();

    glPushMatrix();//eye 1
    glColor3f(0, 0, 0.0); // black
    glTranslatef(0.0, 1, 0.12);
    glScalef(0.17, 0.17, 0.17);
    square();
    glPopMatrix();

    glPushMatrix();//eye 2
    glColor3f(0, 0, 0.0); // black
    glTranslatef(0.05, 1, 0.12);
    glScalef(0.17, 0.17, 0.17);
    square();
    glPopMatrix();

    glPushMatrix();//mouth
    glColor3f(0, 0, 0.0); // black
    glTranslatef(0.00, 1, 0.12);
    glScalef(0.17, 0.17, 0.17);
    mouth();
    glPopMatrix();

}

void Character3()
{
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.2);
    glScalef(2.5, 3, 1.5);
    glColor3f(0.0, 0.75, 0.75);
    glutSolidCube(0.3);
    glPopMatrix();

    //neck
    glPushMatrix();
    glTranslatef(0.0, 0.525, 0.24);
    glScalef(0.5, 0.5, 0.5);
    glColor3f(1, 0.68, 0.68);
    glutSolidCube(0.3);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslatef(0.0, 0.75, 0.24);
    glScalef(1.5, 1.0, 1.35);
    glColor3f(1, 0.68, 0.68);
    glutSolidCube(0.3);
    glPopMatrix();

    //right eye
    glPushMatrix();
    glTranslatef(0.15, (3 + 1 + 2 * 2 / 3) * 0.3 * 0.5, 0.45);
    glScalef(0.15, 0.15, 0.05);
    glColor3f(0.7, 0.7, 0.7);
    glutSolidCube(0.3);
    glPopMatrix();

    //left eye
    glPushMatrix();
    glTranslatef(-0.15, (3 + 1 + 2 * 2 / 3) * 0.3 * 0.5, 0.45);
    glScalef(0.15, 0.15, 0.05);
    glColor3f(0.7, 0.7, 0.7);
    glutSolidCube(0.3);
    glPopMatrix();

    //mouth n nose
    glPushMatrix();
    glTranslatef(0, 0.735, 0.45);
    glScalef(0.15, 0.15, 0);
    glColor3f(0, 0, 0);
    glutSolidCube(0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.66, 0.45); //until here
    glScalef(0.85, 0.15, 0);
    glColor3f(0, 0, 0);
    glutSolidCube(0.3);
    glPopMatrix();

    //left
    glPushMatrix();
    glTranslatef(-0.45, 0.2625, 0.225);
    glScalef(0.5, 1.25, 0.5);
    glColor3f(0.0, 0.85, 0.85);
    glutSolidCube(0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.45, -0.15, 0.225);
    glScalef(0.5, 1.25, 0.5);
    glColor3f(1, 0.7, 0.7);
    glutSolidCube(0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.45, 0.0375, 0.225);
    glScalef(0.5, 0.25, 0.5);
    glColor3f(1, 0.5, 0.5);
    glutSolidCube(0.3);
    glPopMatrix();

    //rightArm
    glPushMatrix();
    glTranslatef(0.45, 0.2625, 0.225);
    glScalef(0.5, 1.25, 0.5);
    glColor3f(0.0, 0.85, 0.85);
    glutSolidCube(0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.45, -0.15, 0.225);
    glScalef(0.5, 1.25, 0.5);
    glColor3f(1, 0.7, 0.7);
    glutSolidCube(0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.45, 0.0375, 0.225);
    glScalef(0.5, 0.25, 0.5);
    glColor3f(1, 0.5, 0.5);
    glutSolidCube(0.3);
    glPopMatrix();

    //leftLeg
    glPushMatrix();
    glTranslatef(-0.2625, -0.675, 0.225);
    glScalef(0.75, 1.5, 0.5);
    glColor3f(0, 0, 0.7);
    glutSolidCube(0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.2625, -1.05, 0.225);
    glScalef(0.75, 1.5, 0.5);
    glColor3f(1, 0.7, 0.7);
    glutSolidCube(0.3);

    glScalef(1.25, 0.25, 1.0);
    glTranslatef(0, -0.75, 0);
    glColor3f(0.3, 0, 0);
    if (isWalking) {
        // Adjust leg angle for walking animation
        glRotatef(-walkingLegAngle, 1.0, 0.0, 0.0);
    }
    glutSolidCube(0.2);
    glPopMatrix();

    //rightLeg
    glPushMatrix();
    glTranslatef(0.2625, -0.675, 0.225);
    glScalef(0.75, 1.5, 0.5);
    glColor3f(0, 0, 0.7);
    glutSolidCube(0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.2625, -1.05, 0.225);
    glScalef(0.75, 1.5, 0.5);
    glColor3f(1, 0.7, 0.7);
    glutSolidCube(0.3);

    glScalef(1.25, 0.25, 1.0);
    glTranslatef(0, -0.75, 0);
    glColor3f(0.3, 0, 0);
    if (isWalking) {
        // Adjust leg angle for walking animation
        glRotatef(-walkingLegAngle, 1.0, 0.0, 0.0);
    }
    glutSolidCube(0.2);
    glPopMatrix();
}

void drawCharacter3()
{
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f); // Move up 1 unit on Y-axis
    glScalef(0.5f, 0.5f, 0.5f);
    Character3();
    glPopMatrix();
}

void drawGround() {
    glColor3f(0.5f, 0.5f, 0.5f); // Set color to grey
    glBegin(GL_QUADS);
    glVertex3f(-5.0f, 0.0f, -5.0f);
    glVertex3f(5.0f, 0.0f, -5.0f);
    glVertex3f(5.0f, 0.0f, 5.0f);
    glVertex3f(-5.0f, 0.0f, 5.0f);
    glEnd();
}
void timerCallback(int value); // Declaration of the timer callback function

void updateJump() {
    if (isJumping) {
        // Update vertical position
        characterPosY += jumpVelocity;

        if (jumpVelocity > 0.0f && characterPosY >= 2.0f) {
            // Reached the peak of the jump, start slowing down
            jumpVelocity -= gravity * 2.0f; // Decrease the velocity faster
        }

        if (jumpVelocity < 0.0f && characterPosY <= 0.0f) {
            // Character is back on the ground
            // glutTimerFunc(5000, timerCallback, 0); // 5000 milliseconds delay (5 seconds)
            characterPosY = 0.0f; // Reset position
            isJumping = false; // Stop jumping
        }

        // Apply gravity
        jumpVelocity -= gravity;
    }

    if (isMovingUp) {
        characterPosY += cameraSpeed;
        if (characterPosY >= 2.0f) {
            isMovingUp = false;
            isMovingDown = true;
        }
    }
    else if (isMovingDown) {
        characterPosY -= cameraSpeed;
        if (characterPosY <= originalY) {
            characterPosY = originalY; // Reset position to original
            isMovingDown = false;
        }
    }
    if (isWalking) {
        // Animate leg angle for walking
        walkingLegAngle += walkingLegSpeed;
        if (walkingLegAngle > 45.0f || walkingLegAngle < -45.0f) {
            walkingLegSpeed *= -1.0f;
        }
    }
    else {
        // Reset leg angle when not walking
        walkingLegAngle = 0.0f;
    }
    glutPostRedisplay();
}
// Timer callback function to reset the jumping flag after a delay
void timerCallback(int value) {
    // Character is at the peak of the jump, stop for a moment
    if (isJumping && jumpVelocity > 0.0f && characterPosY >= 2.0f) {
        // Stop the jump motion, wait for 1 second, then resume downward motion
        jumpVelocity = 0.0f;
        glutTimerFunc(1000, timerCallback, 0); // 1000 milliseconds delay (1 second)
    }
    else {
        // Otherwise, reset the jumping flag
        isJumping = false;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glLoadIdentity();

    switch (currentView) {
    case FIRST_PERSON:
    {
        // Set the camera position relative to the character's position and orientation
        float cameraOffset = 0.5f; // Adjust this value to change the distance from the character
        float cameraX = characterPosX + sin((cameraAngleY + 180) * M_PI / 180) * cameraOffset;
        float cameraY = characterPosY + 1.6f; // Adjust the camera height to be at the character's eye level
        float cameraZ = characterPosZ + cos((cameraAngleY + 180) * M_PI / 180) * cameraOffset;
        float lookAtX = characterPosX + sin(cameraAngleY * M_PI / 180);
        float lookAtY = characterPosY + 1.6f; // Adjust the look-at point's height to be at the character's eye level
        float lookAtZ = characterPosZ + cos(cameraAngleY * M_PI / 180);

        gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);
        break;
    }

    case THIRD_PERSON:
    {
        // Set the camera position relative to the character's position and orientation
        float cameraX = characterPosX - sin(cameraAngleY * M_PI / 180) * 5.0f;
        float cameraY = cameraPosY + 2.0f;
        float cameraZ = characterPosZ - cos(cameraAngleY * M_PI / 180) * 5.0f;
        float lookAtX = characterPosX;
        float lookAtY = cameraPosY;
        float lookAtZ = characterPosZ;

        gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);
        break;
    }
    case TOP_VIEW:
    {
        // Set the camera position relative to the character's position and orientation
        float cameraX = characterPosX;
        float cameraY = characterPosY + 5.0f;
        float cameraZ = characterPosZ;
        float lookAtX = characterPosX;
        float lookAtY = characterPosY;
        float lookAtZ = characterPosZ + 1.0f; // Look slightly ahead

        gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);
        break;
    }
    }

    // Draw the ground (its position remains fixed)
    drawGround();

    // Apply transformations to move the character
    glPushMatrix();
    glTranslatef(characterPosX, characterPosY, characterPosZ);

    // Draw the character based on the selected character type
    switch (currentCharacter) {
    case 1:
        drawCharacter1();
        break;
    case 2:
        drawCharacter2();
        break;
    case 3:
        drawCharacter3();
        break;
    }

    glPopMatrix(); // Restore the original transformation matrix

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// Function to handle mouse motion when a button is pressed
void mouseMotion(int x, int y) {
    if (isDragging) {
        int dx = x - lastMouseX; // Change in x
        int dy = y - lastMouseY; // Change in y

        // Update camera angles based on mouse movement
        cameraAngleY += dx * 0.2f; // Adjust sensitivity as needed
        cameraAngleX += dy * 0.2f; // Adjust sensitivity as needed

        // Clamp the x-axis angle to prevent flipping
        if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
        if (cameraAngleX < -89.0f) cameraAngleX = -89.0f;

        lastMouseX = x; // Update last mouse positions
        lastMouseY = y;

        glutPostRedisplay(); // Redraw the scene
    }
}

// Function to handle mouse button events
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        }
        else if (state == GLUT_UP) {
            isDragging = false;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Lab Test 2");
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard); // Register keyboard callback function
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutIdleFunc(updateJump);
    glutMouseFunc(mouse);    // Register mouse button callback function
    glutMotionFunc(mouseMotion); // Register mouse motion callback function
    glutMainLoop();
    return 0;
}