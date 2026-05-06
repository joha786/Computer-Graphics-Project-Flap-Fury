#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#define PI 3.14159
using namespace std;

GLfloat birdY = 0.0f;
GLfloat velocity = 0.0f;
GLfloat gravity = -0.0012f;
GLfloat jumpStrength = 0.022f;
GLfloat birdRotation = 0.0f;
GLfloat wingAngle = 0.0f;
GLfloat wingDirection = 1.0f;
GLfloat characterScale = 5.0f;

// Game state
int score = 0;
int currentLevel = 1;
bool gameOver = false;
bool levelUp = false;
float levelUpTimer = 0.0f;

// level configuration
struct LevelConfig {
    float pipeSpeed;
    float gapSize;
    float gravity;
    float jumpStrength;
};

LevelConfig levelConfigs[5] = {
    { 0.004f, 0.50f, -0.0012f, 0.022f },   // Level 1 - Bird
    { 0.006f, 0.44f, -0.0013f, 0.023f },   // Level 2 - Rocket
    { 0.008f, 0.38f, -0.0014f, 0.024f },   // Level 3 - Butterfly
    { 0.010f, 0.32f, -0.0015f, 0.025f },   // Level 4 - UFO
    { 0.013f, 0.26f, -0.0016f, 0.026f },   // Level 5 - Dragon
};

//pipe properties
struct Pipe {
    float x;
    float gapY;
    bool  scored;
};
vector<Pipe> pipes;
float pipeSpacing = 0.7f;
float pipeSpeed   = 0.004f;
float gapSize     = 0.50f;

//cloud properties
struct Cloud {
    float x, y, size, speed;
};
vector<Cloud> clouds;

//star properties
struct Star {
    float x, y, brightness;
};
vector<Star> stars;

bool  gameStarted = false;
float groundOffset = 0.0f;

int cyclePhase = 0;
float phaseTimer = 0.0f;
const float dayDuration = 10.0f;
const float nightDuration = 10.0f;

//Sky colour
GLfloat skyR = 0.20f;
GLfloat skyG = 0.50f;
GLfloat skyB = 0.95f;

//Sun
GLfloat sunY =  0.70f;
GLfloat sunAlpha = 1.0f;

//Moon
GLfloat moonY = -1.10f;
GLfloat moonAlpha = 0.0f;

//colors as variable
GLfloat grassR = 0.45f, grassG = 0.75f, grassB = 0.30f;
GLfloat dirtR = 0.60f,  dirtG = 0.45f,  dirtB = 0.20f;
GLfloat bladeR = 0.35f, bladeG = 0.65f, bladeB = 0.25f;
GLfloat sandR = 0.95f,  sandG = 0.85f,  sandB = 0.70f;
GLfloat pDarkG = 0.65f;
GLfloat pBrightG = 0.75f;
GLfloat cloudBright = 1.0f;
GLfloat starAlpha = 0.0f;

void applyLevelSettings() {
    int idx = currentLevel - 1;
    pipeSpeed = levelConfigs[idx].pipeSpeed;
    gapSize = levelConfigs[idx].gapSize;
    gravity = levelConfigs[idx].gravity;
    jumpStrength = levelConfigs[idx].jumpStrength;
}

void resetGame() {
    birdY = 0.0f;
    velocity = 0.0f;
    birdRotation = 0.0f;
    wingAngle = 0.0f;
    wingDirection= 1.0f;
    score = 0;
    currentLevel = 1;
    gameOver = false;
    levelUp = false;
    levelUpTimer = 0.0f;
    gameStarted = false;
    groundOffset = 0.0f;
    applyLevelSettings();
}

void updateDayNightCycle(float dt) {
    phaseTimer += dt;

    switch (cyclePhase) {
    case 0:
        if (phaseTimer >= dayDuration) { phaseTimer = 0; cyclePhase = 1; }
        break;
    case 1:
        if (skyR > 0.02f) skyR -= 0.0010f;
        if (skyG > 0.02f) skyG -= 0.0015f;
        if (skyB > 0.12f) skyB -= 0.0025f;
        if (sunY  > -1.10f) sunY  -= 0.008f;
        if (sunAlpha > 0.0f) sunAlpha -= 0.004f;
        if (moonY < 0.7f) moonY += 0.008f;
        if (moonAlpha < 1.0f) moonAlpha += 0.004f;
        if (sunAlpha < 0.3f){
            if (starAlpha < 1.0f) starAlpha += 0.01f;
        }

        if (grassR > 0.08f) grassR -= 0.0008f;
        if (grassG > 0.20f) grassG -= 0.0015f;
        if (grassB > 0.08f) grassB -= 0.0005f;
        if (dirtR > 0.10f) dirtR -= 0.0010f;
        if (dirtG > 0.10f) dirtG -= 0.0010f;
        if (dirtB > 0.05f) dirtB -= 0.0005f;
        if (bladeR > 0.05f) bladeR -= 0.0005f;
        if (bladeG > 0.15f) bladeG -= 0.0012f;
        if (bladeB > 0.05f) bladeB -= 0.0005f;
        if (sandR > 0.15f) sandR -= 0.0015f;
        if (sandG > 0.15f) sandG -= 0.0012f;
        if (sandB > 0.10f) sandB  -= 0.0010f;
        if (pDarkG > 0.30f) pDarkG   -= 0.0008f;
        if (pBrightG > 0.40f) pBrightG -= 0.0008f;
        if (cloudBright > 0.60f) cloudBright -= 0.0008f;
        if (skyR <= 0.02f && sunAlpha <= 0.0f) { phaseTimer = 0; cyclePhase = 2; }
        break;
    case 2:
        if (phaseTimer >= nightDuration) { phaseTimer = 0; cyclePhase = 3; }
        break;
    case 3:
        if (skyR < 0.20f) skyR += 0.0010f;
        if (skyG < 0.50f) skyG += 0.0015f;
        if (skyB < 0.95f) skyB += 0.0025f;
        if (moonY > -1.10f) moonY -= 0.008f;
        else moonY = -1.10f;
        if (moonAlpha > 0.0f) moonAlpha -= 0.004f;
        if (sunY < 0.70f) sunY += 0.008f;
        if (sunAlpha < 1.0f) sunAlpha += 0.004f;
        if (starAlpha > 0.0f) starAlpha -= 0.005f;
        if (grassR < 0.45f) grassR += 0.0008f;
        if (grassG < 0.75f) grassG += 0.0015f;
        if (grassB < 0.30f) grassB += 0.0005f;
        if (dirtR < 0.60f) dirtR += 0.0010f;
        if (dirtG < 0.45f) dirtG += 0.0010f;
        if (dirtB < 0.20f) dirtB += 0.0005f;
        if (bladeR < 0.35f) bladeR += 0.0005f;
        if (bladeG < 0.65f) bladeG += 0.0012f;
        if (bladeB < 0.25f) bladeB += 0.0005f;
        if (sandR < 0.95f) sandR += 0.0015f;
        if (sandG < 0.85f) sandG += 0.0012f;
        if (sandB < 0.70f) sandB += 0.0010f;
        if (pDarkG < 0.65f) pDarkG += 0.0008f;
        if (pBrightG < 0.75f) pBrightG += 0.0008f;
        if (cloudBright < 1.0f) cloudBright += 0.0008f;
        if (skyR >= 0.20f && moonAlpha <= 0.0f) { phaseTimer = 0; cyclePhase = 0; }
        break;
    }
}

void drawCircle(float cx, float cy, float r, int segments = 40) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    float angle = 2.0f * PI / segments;
    for (int i = 0; i <= segments; i++)
        glVertex2f(cx + r * cos(i * angle), cy + r * sin(i * angle));
    glEnd();
}

void initPipes() {
    pipes.clear();
    for (int i = 0; i < 4; i++) {
        Pipe p;
        p.x = 1.2f + i * pipeSpacing;
        p.gapY = ((rand() % 80) / 100.0f) - 0.4f;
        p.scored = false;
        pipes.push_back(p);
    }
}

void initClouds() {
    clouds.clear();
    for (int i = 0; i < 6; i++) {
        Cloud c;
        c.x = ((rand() % 300) / 100.0f) - 1.5f;
        c.y = 0.2f + ((rand() % 60) / 100.0f);
        c.size = 0.06f + ((rand() % 40) / 1000.0f);
        c.speed = 0.0003f + ((rand() % 20) / 100000.0f);
        clouds.push_back(c);
    }
}

void initStars() {
    stars.clear();
    srand(42);
    for (int i = 0; i < 60; i++) {
        Star s;
        s.x = ((rand() % 200) / 100.0f) - 1.0f;
        s.y = 0.0f + ((rand() % 100) / 100.0f);
        s.brightness = 0.3f + ((rand() % 70) / 100.0f);
        stars.push_back(s);
    }
}

void drawCloud(float x, float y, float size) {
    glColor3f(cloudBright, cloudBright, cloudBright);
    drawCircle(x, y, size);
    drawCircle(x - size*0.7f, y - size*0.2f, size*0.80f);
    drawCircle(x + size*0.7f, y - size*0.1f, size*0.85f);
    drawCircle(x - size*0.3f, y + size*0.4f, size*0.70f);
    drawCircle(x + size*0.4f, y + size*0.3f, size*0.75f);
}

void drawFlowers() {
    float flowerX[] = { -0.85f, -0.55f, -0.20f, 0.10f, 0.40f, 0.70f, 0.90f };
    float petalR[] = { 1.0f, 1.0f, 0.8f, 1.0f, 0.6f, 1.0f, 0.9f };
    float petalG[] = { 0.2f, 0.6f, 0.2f, 0.4f, 0.2f, 0.8f, 0.3f };
    float petalB[] = { 0.8f, 0.2f, 0.9f, 0.2f, 1.0f, 0.2f, 0.6f };
    int numFlowers = 7;

    for (int i = 0; i < numFlowers; i++) {
        float fx = flowerX[i] - groundOffset * 0.5f;
        float fy = -0.78f;

        //stem
        glColor3f(0.20f, 0.55f, 0.10f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        glVertex2f(fx, fy);
        glVertex2f(fx, fy + 0.06f);
        glEnd();

        // Petals
        glColor3f(petalR[i], petalG[i], petalB[i]);
        float pr = 0.012f;
        float cy = fy + 0.06f;
        for (int p = 0; p < 6; p++) {
            float angle = p * (2.0f * PI / 6);
            drawCircle(fx + 0.018f * cos(angle), cy + 0.018f * sin(angle), pr, 10);
        }

        //center
        glColor3f(1.0f, 0.90f, 0.0f);
        drawCircle(fx, cy, 0.010f, 12);
    }
}

void drawBackground() {
    glBegin(GL_QUADS);
    glColor3f(skyR * 0.6f, skyG * 0.6f, skyB * 0.8f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(skyR, skyG, skyB);
    glVertex2f( 1, 0.0f);
    glVertex2f(-1, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(skyR, skyG, skyB);
    glVertex2f(-1,  0.0f);
    glVertex2f( 1,  0.0f);
    glColor3f(sandR, sandG, sandB);
    glVertex2f( 1, -0.7f);
    glVertex2f(-1, -0.7f);
    glEnd();

    if (starAlpha > 0.0f) {
        for (int i = 0; i < (int)stars.size(); i++) {
            glColor3f(1.0f, 1.0f, 1.0f);
            drawCircle(stars[i].x, stars[i].y, 0.006f, 8);
        }
    }

    if (sunAlpha > 0.01f) {
        float sa = sunAlpha;
        glColor3f(1.0f*sa, 0.85f*sa, 0.20f*sa);
        drawCircle(0.7f, sunY, 0.10f);
        glColor3f(1.0f*sa, 0.95f*sa, 0.20f*sa);
        drawCircle(0.7f, sunY, 0.08f);
        glColor3f(1.0f*sa, 0.90f*sa, 0.00f*sa);
        drawCircle(0.7f, sunY, 0.06f);
    }

    if (moonAlpha > 0.01f) {
        float ma = moonAlpha;
        glColor3f(0.70f*ma, 0.70f*ma, 0.80f*ma);
        drawCircle(-0.7f, moonY, 0.10f);
        glColor3f(0.92f*ma, 0.92f*ma, 0.88f*ma);
        drawCircle(-0.7f, moonY, 0.085f);
        glColor3f((skyR+0.03f)*ma,(skyG+0.03f)*ma,(skyB+0.06f)*ma);
        drawCircle(-0.665f, moonY+0.015f, 0.065f);
    }

    for (int i = 0; i < (int)clouds.size(); i++){
        drawCloud(clouds[i].x, clouds[i].y, clouds[i].size);
    }

    glColor3f(grassR, grassG, grassB);
    glBegin(GL_QUADS);
    glVertex2f(-1, -0.70f); glVertex2f(1, -0.70f);
    glVertex2f( 1, -0.78f); glVertex2f(-1,-0.78f);
    glEnd();

    glColor3f(dirtR, dirtG, dirtB);
    glBegin(GL_QUADS);
    glVertex2f(-1, -0.78f); glVertex2f(1, -0.78f);
    glVertex2f( 1, -1.0f);  glVertex2f(-1,-1.0f);
    glEnd();

    glColor3f(bladeR, bladeG, bladeB);
    for (float x = -1.0f + groundOffset; x < 1.2f; x += 0.04f) {
        glBegin(GL_TRIANGLES);
        glVertex2f(x,        -0.78f);
        glVertex2f(x+0.01f,  -0.70f);
        glVertex2f(x+0.02f,  -0.78f);
        glEnd();
    }
    drawFlowers();
}

//level 1: Bird
void drawBird() {
    glPushMatrix();
    glTranslatef(-0.2f, birdY, 0.0f);
    glRotatef(birdRotation, 0, 0, 1);
    glScalef(characterScale, characterScale, 1.0f);

    if (gameStarted) {
        wingAngle += wingDirection * 8.0f;
        if (wingAngle >  25.0f || wingAngle < -25.0f) wingDirection *= -1.0f;
    }

    glColor3f(1.0f, 0.85f, 0.0f); drawCircle(0.0f,  0.0f,  0.055f, 40);
    glColor3f(1.0f, 0.75f, 0.0f); drawCircle(-0.01f,-0.015f,0.04f,  30);
    glColor3f(1.0f, 0.95f, 0.7f); drawCircle(0.005f,-0.02f, 0.025f, 25);

    glPushMatrix();
    glRotatef(wingAngle, 0, 0, 1);
    glColor3f(1.0f, 0.75f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.005f,  0.0f);
    glVertex2f(0.04f,  -0.012f);
    glVertex2f(0.035f, -0.035f);
    glVertex2f(0.015f, -0.038f);
    glEnd();
    glPopMatrix();

    glColor3f(0.9f, 0.65f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.045f,  0.01f);
    glVertex2f(-0.07f,   0.02f);
    glVertex2f(-0.065f, -0.01f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f); drawCircle(0.025f, 0.015f, 0.018f, 20);
    glColor3f(0.0f, 0.0f, 0.0f); drawCircle(0.03f,  0.015f, 0.010f, 20);
    glColor3f(1.0f, 1.0f, 1.0f); drawCircle(0.033f, 0.020f, 0.004f, 10);

    glColor3f(1.0f, 0.60f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.045f, 0.015f);
    glVertex2f(0.045f, 0.000f);
    glVertex2f(0.080f, 0.0075f);
    glEnd();

    glPopMatrix();
}

// level 2: rocket
void drawRocket() {
    glPushMatrix();
    glTranslatef(-0.2f, birdY, 0.0f);
    glRotatef(birdRotation, 0, 0, 1);
    glScalef(characterScale, characterScale, 1.0f);

    // Body
    glColor3f(0.85f, 0.85f, 0.95f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.045f, -0.06f);
    glVertex2f( 0.045f, -0.06f);
    glVertex2f( 0.045f,  0.04f);
    glVertex2f(-0.045f,  0.04f);
    glEnd();

    // Nose cone
    glColor3f(0.95f, 0.30f, 0.20f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.045f, 0.04f);
    glVertex2f( 0.045f, 0.04f);
    glVertex2f( 0.000f, 0.09f);
    glEnd();

    // Window
    glColor3f(0.60f, 0.90f, 1.00f);
    drawCircle(0.0f, 0.01f, 0.022f, 20);
    glColor3f(0.20f, 0.50f, 0.85f);
    drawCircle(0.0f, 0.01f, 0.014f, 20);

    // Left fin
    glColor3f(0.95f, 0.30f, 0.20f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.045f, -0.06f);
    glVertex2f(-0.080f, -0.09f);
    glVertex2f(-0.045f, -0.02f);
    glEnd();

    // Right fin
    glBegin(GL_TRIANGLES);
    glVertex2f( 0.045f, -0.06f);
    glVertex2f( 0.080f, -0.09f);
    glVertex2f( 0.045f, -0.02f);
    glEnd();

    // Flame (animated with wingAngle for flicker)
    float flicker = 0.04f + 0.015f * sin(wingAngle * 0.2f);
    glColor3f(1.0f, 0.70f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.025f, -0.06f);
    glVertex2f( 0.025f, -0.06f);
    glVertex2f( 0.000f, -0.06f - flicker * 2.0f);
    glEnd();
    glColor3f(1.0f, 0.30f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.015f, -0.06f);
    glVertex2f( 0.015f, -0.06f);
    glVertex2f( 0.000f, -0.06f - flicker);
    glEnd();

    glPopMatrix();
}

//level 3: butterfly
void drawButterfly() {
    glPushMatrix();
    glTranslatef(-0.2f, birdY, 0.0f);
    glRotatef(birdRotation * 0.5f, 0, 0, 1);
    glScalef(characterScale, characterScale, 1.0f);

    if (gameStarted) {
        wingAngle += wingDirection * 10.0f;
        if (wingAngle >  40.0f || wingAngle < -40.0f) wingDirection *= -1.0f;
    }

    float wa = wingAngle * PI / 180.0f;
    float wScale = cos(wa);

    // Upper wings
    glColor4f(0.95f, 0.40f, 0.80f, 0.85f);
    glPushMatrix();
    glScalef(wScale, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.00f,  0.01f);
    glVertex2f(0.07f,  0.07f);
    glVertex2f(0.09f,  0.00f);
    glVertex2f(0.05f, -0.03f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f( 0.00f,  0.01f);
    glVertex2f(-0.07f,  0.07f);
    glVertex2f(-0.09f,  0.00f);
    glVertex2f(-0.05f, -0.03f);
    glEnd();
    glPopMatrix();

    // Lower wings
    glColor4f(0.80f, 0.20f, 0.65f, 0.85f);
    glPushMatrix();
    glScalef(wScale, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.00f, -0.01f);
    glVertex2f(0.06f, -0.05f);
    glVertex2f(0.04f, -0.09f);
    glVertex2f(0.01f, -0.07f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f( 0.00f, -0.01f);
    glVertex2f(-0.06f, -0.05f);
    glVertex2f(-0.04f, -0.09f);
    glVertex2f(-0.01f, -0.07f);
    glEnd();
    glPopMatrix();

    // Wing patterns (dots)
    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircle( 0.04f,  0.03f, 0.010f, 12);
    drawCircle(-0.04f,  0.03f, 0.010f, 12);
    drawCircle( 0.03f, -0.04f, 0.008f, 12);
    drawCircle(-0.03f, -0.04f, 0.008f, 12);

    // Body
    glColor3f(0.25f, 0.15f, 0.05f);
    drawCircle(0.0f,  0.005f, 0.015f, 20);
    drawCircle(0.0f, -0.025f, 0.012f, 20);
    drawCircle(0.0f, -0.050f, 0.009f, 20);

    // Antennae
    glColor3f(0.25f, 0.15f, 0.05f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(0.005f, 0.018f); glVertex2f(0.025f,  0.055f);
    glVertex2f(-0.005f,0.018f); glVertex2f(-0.025f, 0.055f);
    glEnd();
    drawCircle( 0.025f, 0.055f, 0.005f, 10);
    drawCircle(-0.025f, 0.055f, 0.005f, 10);

    glPopMatrix();
}

//level 4: UFO
void drawUFO() {
    glPushMatrix();
    glTranslatef(-0.2f, birdY, 0.0f);
    glRotatef(birdRotation * 0.3f, 0, 0, 1);
    glScalef(characterScale, characterScale, 1.0f);

    // Tractor beam (faint triangle below)
    glColor4f(0.60f, 1.0f, 0.60f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.06f, -0.03f);
    glVertex2f( 0.06f, -0.03f);
    glVertex2f( 0.00f, -0.13f);
    glEnd();

    // Saucer body
    glColor3f(0.60f, 0.65f, 0.75f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 40; i++) {
        float a = 2.0f * PI * i / 40;
        glVertex2f(0.09f * cos(a), 0.025f * sin(a));
    }
    glEnd();

    // Rim highlight
    glColor3f(0.80f, 0.85f, 0.95f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 40; i++) {
        float a = 2.0f * PI * i / 40;
        glVertex2f(0.09f * cos(a), 0.025f * sin(a));
    }
    glEnd();

    // Dome
    glColor3f(0.40f, 0.85f, 0.95f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 30; i++) {
        float a = PI * i / 30;
        glVertex2f(0.045f * cos(a), 0.025f + 0.04f * sin(a));
    }
    glEnd();

    // Dome highlight
    glColor3f(0.75f, 0.95f, 1.00f);
    drawCircle(-0.010f, 0.050f, 0.010f, 15);

    // Lights (blinking with wingAngle)
    float blink = (sin(wingAngle * 0.15f) > 0) ? 1.0f : 0.3f;
    glColor3f(1.0f * blink, 0.0f, 0.0f); drawCircle(-0.07f, 0.0f, 0.008f, 12);
    glColor3f(0.0f, 1.0f * blink, 0.0f); drawCircle( 0.00f,-0.02f,0.008f, 12);
    glColor3f(0.0f, 0.0f, 1.0f * blink); drawCircle( 0.07f, 0.0f, 0.008f, 12);

    glPopMatrix();
}

//level 5: Dragon
void drawDragon() {
    glPushMatrix();
    glTranslatef(-0.2f, birdY, 0.0f);
    glRotatef(birdRotation * 0.6f, 0, 0, 1);
    glScalef(characterScale, characterScale, 1.0f);

    if (gameStarted) {
        wingAngle += wingDirection * 7.0f;
        if (wingAngle >  30.0f || wingAngle < -30.0f) wingDirection *= -1.0f;
    }

    // Tail
    glColor3f(0.70f, 0.15f, 0.10f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.02f,  0.01f);
    glVertex2f(-0.05f,  0.02f);
    glVertex2f(-0.09f, -0.01f);
    glVertex2f(-0.06f, -0.03f);
    glVertex2f(-0.03f, -0.02f);
    glEnd();
    // Tail spike
    glColor3f(0.90f, 0.60f, 0.10f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.09f, -0.01f);
    glVertex2f(-0.11f,  0.01f);
    glVertex2f(-0.07f,  0.02f);
    glEnd();

    // Body
    glColor3f(0.80f, 0.15f, 0.10f);
    drawCircle(0.0f, 0.0f, 0.055f, 40);
    glColor3f(0.70f, 0.12f, 0.08f);
    drawCircle(-0.01f, -0.01f, 0.042f, 30);

    // Belly scales
    glColor3f(0.95f, 0.75f, 0.50f);
    drawCircle(0.005f, -0.015f, 0.028f, 25);

    // Wings
    glPushMatrix();
    glRotatef(wingAngle, 0, 0, 1);
    glColor3f(0.85f, 0.20f, 0.15f);
    glBegin(GL_POLYGON);
    glVertex2f(0.01f,  0.02f);
    glVertex2f(0.07f,  0.07f);
    glVertex2f(0.09f,  0.03f);
    glVertex2f(0.06f, -0.01f);
    glVertex2f(0.03f,  0.00f);
    glEnd();
    // Wing membrane lines
    glColor3f(0.70f, 0.12f, 0.10f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(0.01f, 0.02f); glVertex2f(0.07f, 0.07f);
    glVertex2f(0.01f, 0.02f); glVertex2f(0.09f, 0.03f);
    glVertex2f(0.01f, 0.02f); glVertex2f(0.06f,-0.01f);
    glEnd();
    glPopMatrix();

    // Eye
    glColor3f(1.0f, 1.0f, 0.0f); drawCircle(0.025f,  0.018f, 0.016f, 20);
    glColor3f(0.0f, 0.0f, 0.0f); drawCircle(0.030f,  0.018f, 0.008f, 20);
    glColor3f(1.0f, 1.0f, 1.0f); drawCircle(0.033f,  0.022f, 0.003f, 10);

    // Horn
    glColor3f(0.90f, 0.80f, 0.20f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.010f,  0.050f);
    glVertex2f(0.025f,  0.050f);
    glVertex2f(0.018f,  0.080f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.005f, 0.045f);
    glVertex2f( 0.008f, 0.045f);
    glVertex2f( 0.001f, 0.068f);
    glEnd();

    // Snout
    glColor3f(0.85f, 0.20f, 0.15f);
    glBegin(GL_POLYGON);
    glVertex2f(0.040f,  0.010f);
    glVertex2f(0.080f,  0.015f);
    glVertex2f(0.085f,  0.000f);
    glVertex2f(0.040f, -0.005f);
    glEnd();

    // Nostril
    glColor3f(0.50f, 0.05f, 0.05f);
    drawCircle(0.070f, 0.006f, 0.005f, 10);

    // Fire breath (animated)
    float ff = 0.03f + 0.02f * sin(wingAngle * 0.3f);
    glColor3f(1.00f, 0.80f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.085f,  0.012f);
    glVertex2f(0.085f, -0.002f);
    glVertex2f(0.085f + ff * 3.0f, 0.005f);
    glEnd();
    glColor3f(1.0f, 0.30f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.085f,  0.008f);
    glVertex2f(0.085f,  0.001f);
    glVertex2f(0.085f + ff * 1.8f, 0.005f);
    glEnd();

    glPopMatrix();
}

//level character
void drawCharacter() {
    switch (currentLevel) {
        case 1: drawBird();
        break;
        case 2: drawRocket();
        break;
        case 3: drawButterfly();
        break;
        case 4: drawUFO();
        break;
        case 5: drawDragon();
        break;
        default: drawBird();
        break;
    }
}

void drawPipes() {
    float pipeWidth = 0.12f;
    float capHeight = 0.05f;
    float capWidth  = 0.15f;

    for (int i = 0; i < (int)pipes.size(); i++) {
        // top body
        glColor3f(0.20f, pDarkG, 0.20f);
        glBegin(GL_QUADS);
        glVertex2f(pipes[i].x,1);
        glVertex2f(pipes[i].x + pipeWidth, 1);
        glVertex2f(pipes[i].x + pipeWidth, pipes[i].gapY + gapSize/2 + capHeight);
        glVertex2f(pipes[i].x,pipes[i].gapY + gapSize/2 + capHeight);
        glEnd();

        // top highlight
        glColor3f(0.25f, pBrightG, 0.25f);
        glBegin(GL_QUADS);
        glVertex2f(pipes[i].x + 0.01f, 1);
        glVertex2f(pipes[i].x + 0.03f, 1);
        glVertex2f(pipes[i].x + 0.03f, pipes[i].gapY + gapSize/2 + capHeight);
        glVertex2f(pipes[i].x + 0.01f, pipes[i].gapY + gapSize/2 + capHeight);
        glEnd();

        // top cap
        glColor3f(0.25f, pBrightG, 0.25f);
        glBegin(GL_QUADS);
        glVertex2f(pipes[i].x - 0.03f, pipes[i].gapY + gapSize/2 + capHeight);
        glVertex2f(pipes[i].x + capWidth, pipes[i].gapY + gapSize/2 + capHeight);
        glVertex2f(pipes[i].x + capWidth, pipes[i].gapY + gapSize/2);
        glVertex2f(pipes[i].x - 0.03f, pipes[i].gapY + gapSize/2);
        glEnd();

        // bottom body
        glColor3f(0.20f, pDarkG, 0.20f);
        glBegin(GL_QUADS);
        glVertex2f(pipes[i].x,pipes[i].gapY - gapSize/2 - capHeight);
        glVertex2f(pipes[i].x + pipeWidth, pipes[i].gapY - gapSize/2 - capHeight);
        glVertex2f(pipes[i].x + pipeWidth, -0.7f);
        glVertex2f(pipes[i].x, -0.7f);
        glEnd();

        // bottom highlight
        glColor3f(0.25f, pBrightG, 0.25f);
        glBegin(GL_QUADS);
        glVertex2f(pipes[i].x + 0.01f, pipes[i].gapY - gapSize/2 - capHeight);
        glVertex2f(pipes[i].x + 0.03f, pipes[i].gapY - gapSize/2 - capHeight);
        glVertex2f(pipes[i].x + 0.03f, -0.7f);
        glVertex2f(pipes[i].x + 0.01f, -0.7f);
        glEnd();

        // bottom cap
        glColor3f(0.25f, pBrightG, 0.25f);
        glBegin(GL_QUADS);
        glVertex2f(pipes[i].x - 0.03f, pipes[i].gapY - gapSize/2);
        glVertex2f(pipes[i].x + capWidth, pipes[i].gapY - gapSize/2);
        glVertex2f(pipes[i].x + capWidth, pipes[i].gapY - gapSize/2 - capHeight);
        glVertex2f(pipes[i].x - 0.03f, pipes[i].gapY - gapSize/2 - capHeight);
        glEnd();
    }
}

void drawText(float x, float y, string text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (int i = 0; i < (int)text.length(); i++)
        glutBitmapCharacter(font, text[i]);
}

// ─── HUD: score + level ──────────────────────────────────────────────────────
void drawHUD() {
    // Score
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.95f, 0.88f, "Score: " + to_string(score), GLUT_BITMAP_HELVETICA_18);

    // Level badge
    string lvlStr = "LEVEL " + to_string(currentLevel);
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText( 0.55f, 0.88f, lvlStr, GLUT_BITMAP_HELVETICA_18);

    // Points to next level
    if (currentLevel < 5) {
        int need = currentLevel * 10 - score;
        if (need < 0) need = 0;
        glColor3f(0.9f, 0.9f, 0.9f);
        string nextStr = "Next level: " + to_string(need) + " pts";
        drawText(-0.95f, 0.78f, nextStr, GLUT_BITMAP_HELVETICA_12);
    } else {
        glColor3f(1.0f, 0.85f, 0.0f);
        drawText(-0.95f, 0.78f, "MAX LEVEL!", GLUT_BITMAP_HELVETICA_12);
    }
}


//shows when level up occurs
void drawLevelUpBanner() {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(-0.55f,  0.20f);
    glVertex2f( 0.55f,  0.20f);
    glVertex2f( 0.55f, -0.10f);
    glVertex2f(-0.55f, -0.10f);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(0.20f, 1.0f, 0.40f);
    drawText(-0.21f, 0.10f, "LEVEL UP!", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1.0f, 1.0f, 1.0f);
    string msg = "Now: LEVEL " + to_string(currentLevel);
    drawText(-0.22f, -0.02f, msg, GLUT_BITMAP_HELVETICA_18);
}

//shows when game is over
void drawGameOver() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glBegin(GL_QUADS);
    glVertex2f(-0.55f,  0.35f);
    glVertex2f( 0.55f,  0.35f);
    glVertex2f( 0.55f, -0.30f);
    glVertex2f(-0.55f, -0.30f);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(1.0f, 0.20f, 0.20f);
    drawText(-0.22f,  0.22f, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.20f,  0.06f, "Score: " + to_string(score), GLUT_BITMAP_HELVETICA_18);
    drawText(-0.22f, -0.04f, "Level: " + to_string(currentLevel), GLUT_BITMAP_HELVETICA_18);

    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(-0.28f, -0.16f, "SPACE = Play Again", GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.80f, 0.80f, 0.80f);
    drawText(-0.10f, -0.24f, "ESC = Quit", GLUT_BITMAP_HELVETICA_12);
}
/*
// ─── Collision detection ─────────────────────────────────────────────────────
bool checkCollision() {
    float bx   = -0.2f;
    float by   =  birdY;
    float brad =  0.048f;   // slightly smaller than visual for fairness

    // Ground / ceiling
    if (by - brad < -0.70f) return true;
    if (by + brad >  1.00f) return true;

    float pipeWidth = 0.12f;
    float capWidth  = 0.15f;
    float capHeight = 0.05f;

    for (int i = 0; i < (int)pipes.size(); i++) {
        float px  = pipes[i].x;
        float py  = pipes[i].gapY;
        float top = py + gapSize / 2;
        float bot = py - gapSize / 2;

        // Effective pipe horizontal extent (cap is wider)
        float leftEdge  = px - 0.03f;
        float rightEdge = px + capWidth;

        bool inXRange = (bx + brad > leftEdge) && (bx - brad < rightEdge);
        if (!inXRange) continue;

        // Collide with top pipe (cap region)
        if (by + brad > top - capHeight && bx + brad > leftEdge && bx - brad < rightEdge)
            if (by + brad > top - capHeight) {
                // bird hits top cap or top body
                if (by + brad > top - capHeight) return true;
            }

        // Simpler AABB for top pipe body
        if (by + brad > top) return true;
        if (by - brad < bot) return true;
    }
    return false;
}
*/
// Refined AABB collision (cleaner)
bool checkCollisionAABB() {
    float bx = -0.2f;
    float by = birdY;
    float brad = 0.045f;

    // Ground and ceiling
    if (by - brad < -0.70f) return true;
    if (by + brad >  1.00f) return true;

    float capWidth  = 0.15f;
    float capHeight = 0.05f;

    for (int i = 0; i < (int)pipes.size(); i++) {
        float px  = pipes[i].x;
        float py  = pipes[i].gapY;
        float top = py + gapSize / 2;
        float bot = py - gapSize / 2;

        //left edge of cap, right edge of body
        float left  = px - 0.03f;
        float right = px + capWidth;

        bool inX = (bx + brad > left) && (bx - brad < right);
        if (!inX) continue;

        // Collision when chracter above gap top OR below gap bottom
        if (by + brad > top + capHeight) return true; // top body (add capHeight because cap sticks out)
        if (by + brad > top) return true; // top cap zone
        if (by - brad < bot) return true; // bottom cap zone
        if (by - brad < bot - capHeight) return true; // bottom body
    }
    return false;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground();
    drawPipes();
    drawCharacter();

    if (gameStarted && !gameOver) {
        drawHUD();
    }

    if (levelUp) {
        drawLevelUpBanner();
    }

    if (!gameStarted && !gameOver) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.60f);
        glBegin(GL_QUADS);
        glVertex2f(-0.55f,  0.35f);
        glVertex2f( 0.55f,  0.35f);
        glVertex2f( 0.55f, -0.28f);
        glVertex2f(-0.55f, -0.28f);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(-0.20f,  0.20f, "FLAP FURY", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(0.90f, 0.90f, 1.0f);
        drawText(-0.38f,  0.06f, "5 Levels  |  5 Characters", GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-0.26f, -0.06f, "Press SPACE to Start", GLUT_BITMAP_HELVETICA_18);
        glColor3f(0.70f, 0.70f, 0.70f);
        drawText(-0.12f, -0.18f, "ESC = Quit", GLUT_BITMAP_HELVETICA_12);
    }

    if (gameOver) {
        drawGameOver();
    }

    glFlush();
}

void update(int) {
    float dt = 16.0f / 1000.0f;

    updateDayNightCycle(dt);

    for (int i = 0; i < (int)clouds.size(); i++) {
        clouds[i].x -= clouds[i].speed;
        if (clouds[i].x < -1.5f) {
            clouds[i].x = 1.3f;
            clouds[i].y = 0.2f + ((rand() % 60) / 100.0f);
        }
    }

    if (gameStarted && !gameOver) {

        // Level-up banner timer
        if (levelUp) {
            levelUpTimer -= dt;
            if (levelUpTimer <= 0.0f) levelUp = false;
        }

        velocity += gravity;
        birdY += velocity;

        birdRotation = velocity * 800.0f;
        if (birdRotation > 30)  birdRotation =  30;
        if (birdRotation < -90)  birdRotation = -90;

        // moving pipes
        for (int i = 0; i < (int)pipes.size(); i++){
            pipes[i].x -= pipeSpeed;
        }

        // Scoring: award 1 point when bird passes pipe center
        for (int i = 0; i < (int)pipes.size(); i++) {
            if (!pipes[i].scored && pipes[i].x + 0.06f < -0.2f) {
                pipes[i].scored = true;
                score++;

                // Level-up every 10 points (caps at level 5)
                int newLevel = (score / 2) + 1;
                if (newLevel > 5) newLevel = 5;
                if (newLevel > currentLevel) {
                    currentLevel = newLevel;
                    applyLevelSettings();
                    levelUp = true;
                    levelUpTimer = 2.0f;
                    // Re-init pipes for new gap size
                    float lastX = pipes.back().x;
                    pipes.clear();
                    for (int j = 0; j < 4; j++) {
                        Pipe np;
                        np.x = lastX + (j + 1) * pipeSpacing;
                        np.gapY = ((rand() % 80) / 100.0f) - 0.4f;
                        np.scored = false;
                        pipes.push_back(np);
                    }
                }
            }
        }

        // Recycle pipes
        if (pipes[0].x < -1.3f) {
            pipes.erase(pipes.begin());
            Pipe newPipe;
            newPipe.x = pipes.back().x + pipeSpacing;
            newPipe.gapY = ((rand() % 80) / 100.0f) - 0.4f;
            newPipe.scored = false;
            pipes.push_back(newPipe);
        }

        groundOffset -= pipeSpeed;
        if (groundOffset < -0.04f) groundOffset = 0.0f;

        // Collision check
        if (checkCollisionAABB()) {   //axis aligned bounding box
            gameOver = true;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int, int) {
    if (key == 32) {
        if (gameOver) {
            // Restart
            resetGame();
            initPipes();
            initClouds();
            initStars();
        } else if (!gameStarted) {
            gameStarted = true;
            velocity = jumpStrength;
        } else {
            velocity = jumpStrength;
        }
    }
    if (key == 27) {   // ESC
        exit(0);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Flap Fury");

    glClearColor(0.20f, 0.50f, 0.95f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initClouds();
    initPipes();
    initStars();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);
    glutMainLoop();

    return 0;
}
