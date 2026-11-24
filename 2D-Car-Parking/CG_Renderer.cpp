#include "CG_Renderer.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

Renderer::Renderer(int width, int height)
    : screenWidth(width), screenHeight(height), pulseTimer(0.0f) {}

void Renderer::initOpenGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.08f, 0.10f, 0.14f, 1.0f);
}

// CG CO4 & CO5: Viewport handling and 2D orthographic projection with coordinate alignment
void Renderer::resize(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)w, (double)h, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Renderer::drawText(float x, float y, const std::string& text, void* font) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font ? font : GLUT_BITMAP_HELVETICA_12, c);
    }
}

void Renderer::updatePulse(float dt) {
    pulseTimer += dt * 4.0f;
}

// CG CO3: 2D button primitive construction using GL_QUADS and GL_LINE_LOOP
void Renderer::drawButton(float x, float y, float w, float h, const std::string& label,
                          float r, float g, float b) {
    glColor4f(r, g, b, 0.92f);
    glRectf(x, y, x + w, y + h);

    glColor3f(std::min(1.0f, r + 0.35f), std::min(1.0f, g + 0.35f), std::min(1.0f, b + 0.35f));
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    float textX = x + (w / 2.0f) - (label.length() * 3.8f);
    float textY = y + (h / 2.0f) + 5.0f;
    drawText(textX, textY, label, GLUT_BITMAP_HELVETICA_12);
}

void Renderer::renderScene(Car& car, ParkingLot& lot, COAProcessor& coa,
                           const std::string& gameState, float timeRemainingSec,
                           int currentLevel, int maxCollisions, const std::string& playerName,
                           int lastLevelScore, const std::string& failReason,
                           const std::string& alertMsg, float alertTimer) {
    glClear(GL_COLOR_BUFFER_BIT);

    float pulseScale = 1.0f + 0.08f * std::sin(pulseTimer);

    if (gameState == "MENU") {
        drawMainMenu(playerName);
        return;
    }

    // 1. Render Asphalt Parking Lot with Road Textures and Guide Path (CG CO4 & CO6)
    lot.render(pulseScale);

    // 2. Render Player Car with Light Color Effects (CG CO6)
    car.render();

    // 3. Render HUD and On-Screen Occupied Alert Banner
    drawHUD(car, lot, timeRemainingSec, currentLevel, maxCollisions, playerName, alertMsg, alertTimer, coa.getDecisionState());

    // 4. Popups for Win / Fail / Won
    if (gameState == "LEVEL_WIN") {
        float timeTaken = 30.0f - timeRemainingSec;
        drawLevelWinPopup(currentLevel, lastLevelScore, timeTaken, car.getCollisions(), maxCollisions);
    } else if (gameState == "LEVEL_FAIL") {
        float timeTaken = 30.0f - timeRemainingSec;
        drawLevelFailPopup(currentLevel, failReason, car.getCollisions(), maxCollisions, timeTaken);
    } else if (gameState == "GAME_WON") {
        drawGameWonPopup(car.getScore(), playerName);
    }
}

// CG CO3: Menu interface construction using 2D primitives (GL_QUADS, GL_LINE_LOOP)
void Renderer::drawMainMenu(const std::string& activePlayer) {
    glColor4f(0.06f, 0.09f, 0.14f, 0.95f);
    glRectf(140.0f, 50.0f, 660.0f, 470.0f);

    glColor3f(0.20f, 0.60f, 0.90f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(140.0f, 50.0f);
        glVertex2f(660.0f, 50.0f);
        glVertex2f(660.0f, 470.0f);
        glVertex2f(140.0f, 470.0f);
    glEnd();

    glColor3f(1.0f, 0.85f, 0.20f);
    drawText(270.0f, 95.0f, "2D CAR PARKING GAME", GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.35f, 0.85f, 0.95f);
    drawText(235.0f, 120.0f, "Semester III Integrated Academic Project", GLUT_BITMAP_HELVETICA_10);

    glColor3f(0.40f, 0.95f, 0.50f);
    std::string playerStr = "Active Player: " + (activePlayer.empty() ? "Guest" : activePlayer);
    drawText(310.0f, 150.0f, playerStr, GLUT_BITMAP_HELVETICA_12);

    drawButton(270.0f, 180.0f, 260.0f, 44.0f, "START GAME (Play)", 0.12f, 0.65f, 0.30f);
    drawButton(270.0f, 238.0f, 260.0f, 44.0f, "MANAGE / SELECT PLAYER", 0.18f, 0.45f, 0.75f);
    drawButton(270.0f, 296.0f, 260.0f, 44.0f, "VIEW LEADERBOARD", 0.80f, 0.55f, 0.15f);
    drawButton(270.0f, 354.0f, 260.0f, 44.0f, "EXIT GAME", 0.75f, 0.20f, 0.20f);

    glColor3f(0.60f, 0.70f, 0.80f);
    drawText(200.0f, 440.0f, "COA(CO1-4 in NASM64) | OOP(CO1-4) | PL(CO1-5) | CG(CO1-6)", GLUT_BITMAP_HELVETICA_10);
}

// PL CO4 & CO5: Secure Player Selection, Password Authentication & Account Deletion UI
void Renderer::drawPlayerSelectScreen(const PlayerDatabase& db, const std::string& nameInput,
                                      const std::string& pwdInput, int activeField,
                                      const std::string& feedbackMsg, bool showAuthModal,
                                      const std::string& authPlayerName, const std::string& authPwdInput,
                                      const std::string& authErrorMsg) {
    glColor4f(0.06f, 0.09f, 0.14f, 0.95f);
    glRectf(80.0f, 30.0f, 720.0f, 490.0f);

    glColor3f(0.20f, 0.60f, 0.90f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(80.0f, 30.0f);
        glVertex2f(720.0f, 30.0f);
        glVertex2f(720.0f, 490.0f);
        glVertex2f(80.0f, 490.0f);
    glEnd();

    glColor3f(1.0f, 0.85f, 0.20f);
    drawText(260.0f, 60.0f, "ACCOUNT CREATION & LOGIN (PL CO4/5)", GLUT_BITMAP_HELVETICA_18);

    // Feedback message
    if (!feedbackMsg.empty()) {
        glColor3f(1.0f, 0.35f, 0.35f);
        drawText(260.0f, 80.0f, feedbackMsg, GLUT_BITMAP_HELVETICA_10);
    }

    // Input 1: Username
    glColor3f(0.85f, 0.90f, 0.95f);
    drawText(110.0f, 102.0f, "Username:", GLUT_BITMAP_HELVETICA_12);
    glColor3f(activeField == 0 ? 0.22f : 0.12f, activeField == 0 ? 0.28f : 0.16f, activeField == 0 ? 0.38f : 0.22f);
    glRectf(190.0f, 88.0f, 370.0f, 118.0f);
    glColor3f(activeField == 0 ? 0.35f : 0.40f, activeField == 0 ? 0.85f : 0.55f, 0.85f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(190.0f, 88.0f); glVertex2f(370.0f, 88.0f);
        glVertex2f(370.0f, 118.0f); glVertex2f(190.0f, 118.0f);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    std::string dName = nameInput.empty() ? (activeField == 0 ? "_" : "[Enter name]") : nameInput + (activeField == 0 ? "_" : "");
    drawText(200.0f, 107.0f, dName, GLUT_BITMAP_HELVETICA_12);

    // Input 2: Password
    glColor3f(0.85f, 0.90f, 0.95f);
    drawText(390.0f, 102.0f, "Password:", GLUT_BITMAP_HELVETICA_12);
    glColor3f(activeField == 1 ? 0.22f : 0.12f, activeField == 1 ? 0.28f : 0.16f, activeField == 1 ? 0.38f : 0.22f);
    glRectf(470.0f, 88.0f, 590.0f, 118.0f);
    glColor3f(activeField == 1 ? 0.35f : 0.40f, activeField == 1 ? 0.85f : 0.55f, 0.85f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(470.0f, 88.0f); glVertex2f(590.0f, 88.0f);
        glVertex2f(590.0f, 118.0f); glVertex2f(470.0f, 118.0f);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    std::string masked = std::string(pwdInput.length(), '*');
    std::string dPwd = masked.empty() ? (activeField == 1 ? "_" : "[Password]") : masked + (activeField == 1 ? "_" : "");
    drawText(480.0f, 107.0f, dPwd, GLUT_BITMAP_HELVETICA_12);

    // Add Player Button
    drawButton(605.0f, 88.0f, 95.0f, 30.0f, "+ REGISTER", 0.15f, 0.65f, 0.30f);

    // Divider
    glColor3f(0.25f, 0.35f, 0.50f);
    glBegin(GL_LINES);
        glVertex2f(100.0f, 135.0f); glVertex2f(700.0f, 135.0f);
    glEnd();

    glColor3f(0.35f, 0.85f, 1.0f);
    drawText(110.0f, 155.0f, "SAVED PLAYERS (Password Protected - PL Hash Table):", GLUT_BITMAP_HELVETICA_12);

    // Saved Players List
    const auto& players = db.getPlayers();
    int startY = 175;
    for (size_t i = 0; i < std::min((size_t)5, players.size()); i++) {
        glColor3f(0.12f, 0.16f, 0.24f);
        glRectf(110.0f, (float)startY, 490.0f, (float)(startY + 36));

        glColor3f(0.30f, 0.40f, 0.55f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(110.0f, (float)startY); glVertex2f(490.0f, (float)startY);
            glVertex2f(490.0f, (float)(startY + 36)); glVertex2f(110.0f, (float)(startY + 36));
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        std::stringstream pInfo;
        pInfo << (i + 1) << ". " << players[i].name << "  |  Score: " << players[i].totalScore
              << " pts  |  Cleared: " << players[i].levelsCompleted << "/3";
        drawText(125.0f, (float)(startY + 23), pInfo.str(), GLUT_BITMAP_HELVETICA_12);

        // Play As This Button
        drawButton(505.0f, (float)startY, 110.0f, 36.0f, "PLAY AS THIS", 0.20f, 0.45f, 0.80f);

        // Delete Player Button (User Request)
        drawButton(625.0f, (float)startY, 75.0f, 36.0f, "DELETE", 0.80f, 0.20f, 0.20f);

        startY += 45;
    }

    drawButton(280.0f, 440.0f, 240.0f, 36.0f, "BACK TO MAIN MENU", 0.50f, 0.50f, 0.50f);

    // Password Prompt Modal (When logging in to existing account)
    if (showAuthModal) {
        // Dim background
        glColor4f(0.0f, 0.0f, 0.0f, 0.70f);
        glRectf(0.0f, 0.0f, 800.0f, 520.0f);

        // Modal Box
        glColor4f(0.10f, 0.14f, 0.20f, 0.98f);
        glRectf(200.0f, 160.0f, 600.0f, 340.0f);

        glColor3f(0.30f, 0.70f, 1.0f);
        glLineWidth(2.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(200.0f, 160.0f); glVertex2f(600.0f, 160.0f);
            glVertex2f(600.0f, 340.0f); glVertex2f(200.0f, 340.0f);
        glEnd();

        glColor3f(1.0f, 0.85f, 0.20f);
        drawText(240.0f, 195.0f, "SECURITY: ENTER ACCOUNT PASSWORD", GLUT_BITMAP_HELVETICA_12);

        glColor3f(1.0f, 1.0f, 1.0f);
        std::string promptMsg = "Player: " + authPlayerName;
        drawText(240.0f, 220.0f, promptMsg, GLUT_BITMAP_HELVETICA_12);

        // Modal Password Box
        glColor3f(0.16f, 0.22f, 0.30f);
        glRectf(240.0f, 235.0f, 560.0f, 270.0f);
        glColor3f(0.40f, 0.65f, 0.90f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(240.0f, 235.0f); glVertex2f(560.0f, 235.0f);
            glVertex2f(560.0f, 270.0f); glVertex2f(240.0f, 270.0f);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        std::string mAuth = std::string(authPwdInput.length(), '*') + "_";
        drawText(255.0f, 257.0f, mAuth, GLUT_BITMAP_HELVETICA_12);

        if (!authErrorMsg.empty()) {
            glColor3f(1.0f, 0.25f, 0.25f);
            drawText(240.0f, 290.0f, authErrorMsg, GLUT_BITMAP_HELVETICA_10);
        }

        drawButton(240.0f, 295.0f, 150.0f, 32.0f, "CONFIRM LOGIN", 0.15f, 0.65f, 0.30f);
        drawButton(410.0f, 295.0f, 150.0f, 32.0f, "CANCEL", 0.75f, 0.25f, 0.25f);
    }
}

void Renderer::drawLeaderboardScreen(const PlayerDatabase& db) {
    glColor4f(0.06f, 0.09f, 0.14f, 0.95f);
    glRectf(120.0f, 50.0f, 680.0f, 470.0f);

    glColor3f(0.90f, 0.70f, 0.20f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(120.0f, 50.0f); glVertex2f(680.0f, 50.0f);
        glVertex2f(680.0f, 470.0f); glVertex2f(120.0f, 470.0f);
    glEnd();

    glColor3f(1.0f, 0.85f, 0.20f);
    drawText(300.0f, 90.0f, "HIGH SCORES LEADERBOARD", GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.35f, 0.75f, 1.0f);
    drawText(160.0f, 135.0f, "RANK", GLUT_BITMAP_HELVETICA_12);
    drawText(240.0f, 135.0f, "PLAYER NAME", GLUT_BITMAP_HELVETICA_12);
    drawText(430.0f, 135.0f, "TOTAL SCORE", GLUT_BITMAP_HELVETICA_12);
    drawText(560.0f, 135.0f, "LEVELS CLEARED", GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.30f, 0.40f, 0.55f);
    glBegin(GL_LINES);
        glVertex2f(150.0f, 145.0f); glVertex2f(650.0f, 145.0f);
    glEnd();

    const auto& players = db.getPlayers();
    int rowY = 175;
    for (size_t i = 0; i < std::min((size_t)7, players.size()); i++) {
        if (i == 0) glColor3f(1.0f, 0.85f, 0.20f);
        else if (i == 1) glColor3f(0.85f, 0.85f, 0.90f);
        else if (i == 2) glColor3f(0.80f, 0.50f, 0.20f);
        else glColor3f(0.90f, 0.90f, 0.90f);

        drawText(170.0f, (float)rowY, "#" + std::to_string(i + 1), GLUT_BITMAP_HELVETICA_12);
        drawText(240.0f, (float)rowY, players[i].name, GLUT_BITMAP_HELVETICA_12);
        drawText(450.0f, (float)rowY, std::to_string(players[i].totalScore) + " pts", GLUT_BITMAP_HELVETICA_12);
        drawText(590.0f, (float)rowY, std::to_string(players[i].levelsCompleted) + " / 3", GLUT_BITMAP_HELVETICA_12);
        rowY += 32;
    }

    drawButton(280.0f, 415.0f, 240.0f, 38.0f, "BACK TO MAIN MENU", 0.20f, 0.45f, 0.75f);
}

// CG CO3 & CO4: In-game HUD with Road Dashboard and Occupied Alert Banner
void Renderer::drawHUD(const Car& car, const ParkingLot& lot, float timeRemainingSec,
                       int currentLevel, int maxCollisions, const std::string& playerName,
                       const std::string& alertMsg, float alertTimer, const std::string& parkingState) {
    glColor4f(0.06f, 0.08f, 0.12f, 0.92f);
    glRectf(40.0f, 6.0f, 760.0f, 52.0f);

    glColor3f(0.20f, 0.30f, 0.45f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(40.0f, 6.0f); glVertex2f(760.0f, 6.0f);
        glVertex2f(760.0f, 52.0f); glVertex2f(40.0f, 52.0f);
    glEnd();

    ParkingSlot* target = const_cast<ParkingLot&>(lot).getTargetSlot();
    std::string targetID = target ? target->slotID : "PARK";

    glColor3f(1.0f, 0.85f, 0.20f);
    std::stringstream sLevel;
    sLevel << "LEVEL: " << currentLevel << "/3";
    drawText(50.0f, 26.0f, sLevel.str(), GLUT_BITMAP_HELVETICA_12);

    if (timeRemainingSec <= 10.0f) glColor3f(1.0f, 0.25f, 0.25f);
    else glColor3f(0.30f, 0.90f, 1.0f);
    std::stringstream sTime;
    sTime << "TIME: " << (int)std::ceil(timeRemainingSec) << "s/30s";
    drawText(150.0f, 26.0f, sTime.str(), GLUT_BITMAP_HELVETICA_12);

    if (car.getCollisions() >= maxCollisions - 1) glColor3f(1.0f, 0.25f, 0.25f);
    else glColor3f(0.40f, 0.90f, 0.50f);
    std::stringstream sHits;
    sHits << "HITS: " << car.getCollisions() << "/" << maxCollisions;
    drawText(265.0f, 26.0f, sHits.str(), GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.90f, 0.90f, 0.90f);
    std::stringstream sTgt;
    sTgt << "TARGET: " << targetID << "  |  SCORE: " << car.getScore();
    drawText(360.0f, 26.0f, sTgt.str(), GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.60f, 0.70f, 0.80f);
    std::string playerStr = "Player: " + playerName + "  |  Follow Cyan Trail";
    drawText(50.0f, 44.0f, playerStr, GLUT_BITMAP_HELVETICA_10);

    // Real-time Parking Status Indicator
    if (parkingState == "PARK_SUCCESS_CONFIRMED") {
        glColor3f(0.20f, 1.0f, 0.35f);
        drawText(310.0f, 44.0f, "[STATUS: PARKED! (Hold / Stop to Win)]", GLUT_BITMAP_HELVETICA_12);
    } else if (parkingState == "ROTATION_MISALIGNED") {
        glColor3f(1.0f, 0.85f, 0.20f);
        drawText(310.0f, 44.0f, "[STATUS: IN SLOT - STRAIGHTEN CAR]", GLUT_BITMAP_HELVETICA_12);
    } else {
        glColor3f(0.40f, 0.75f, 0.95f);
        drawText(310.0f, 44.0f, "[STATUS: APPROACHING TARGET SLOT]", GLUT_BITMAP_HELVETICA_10);
    }

    drawButton(590.0f, 12.0f, 78.0f, 30.0f, "RESTART (R)", 0.75f, 0.45f, 0.10f);
    drawButton(676.0f, 12.0f, 76.0f, 30.0f, "MENU (Esc)", 0.75f, 0.15f, 0.15f);

    // On-Screen Occupied Alert Banner (User Request)
    if (alertTimer > 0.0f && !alertMsg.empty()) {
        float alertWidth = 280.0f;
        float ax = 400.0f - alertWidth / 2.0f;
        float ay = 75.0f;

        // Glowing red alert box
        glColor4f(0.85f, 0.10f, 0.10f, 0.92f);
        glRectf(ax, ay, ax + alertWidth, ay + 32.0f);

        glColor3f(1.0f, 0.95f, 0.20f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(ax, ay); glVertex2f(ax + alertWidth, ay);
            glVertex2f(ax + alertWidth, ay + 32.0f); glVertex2f(ax, ay + 32.0f);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(ax + 18.0f, ay + 21.0f, "[!] ALERT: " + alertMsg, GLUT_BITMAP_HELVETICA_12);
    }
}

// Redesigned Modern Level Win Popup with Performance Stars and COA Score Breakdown Card
void Renderer::drawLevelWinPopup(int levelNum, int levelScore, float timeTaken, int collisions, int maxCollisions) {
    // Backdrop
    glColor4f(0.02f, 0.05f, 0.08f, 0.85f);
    glRectf(0.0f, 0.0f, 800.0f, 520.0f);

    // Dialog card
    glColor4f(0.06f, 0.12f, 0.10f, 0.98f);
    glRectf(150.0f, 90.0f, 650.0f, 430.0f);

    glColor3f(0.15f, 0.90f, 0.45f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(150.0f, 90.0f); glVertex2f(650.0f, 90.0f);
        glVertex2f(650.0f, 430.0f); glVertex2f(150.0f, 430.0f);
    glEnd();

    glColor3f(0.20f, 1.0f, 0.50f);
    std::stringstream ssTitle;
    ssTitle << "LEVEL " << levelNum << " COMPLETED!";
    drawText(280.0f, 135.0f, ssTitle.str(), GLUT_BITMAP_HELVETICA_18);

    // Star Rating based on collision count
    glColor3f(1.0f, 0.85f, 0.15f);
    std::string stars = (collisions == 0) ? "[ * * * ] THREE STARS" :
                        (collisions == 1) ? "[ * * - ] TWO STARS" : "[ * - - ] ONE STAR";
    drawText(310.0f, 165.0f, stars, GLUT_BITMAP_HELVETICA_12);

    // Score Breakdown Card
    glColor4f(0.10f, 0.18f, 0.15f, 0.90f);
    glRectf(180.0f, 185.0f, 620.0f, 340.0f);
    glColor3f(0.20f, 0.50f, 0.35f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(180.0f, 185.0f); glVertex2f(620.0f, 185.0f);
        glVertex2f(620.0f, 340.0f); glVertex2f(180.0f, 340.0f);
    glEnd();

    glColor3f(0.95f, 0.95f, 0.95f);
    drawText(200.0f, 215.0f, "COA 80386 Native Assembly Score Breakdown:", GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.40f, 0.90f, 0.50f);
    drawText(220.0f, 245.0f, "+ Base Score (R00):        100 pts", GLUT_BITMAP_HELVETICA_12);

    glColor3f(1.0f, 0.40f, 0.40f);
    std::string colStr = "- Collision Penalty (R01):  " + std::to_string(collisions * 20) + " pts (" + std::to_string(collisions) + " hits)";
    drawText(220.0f, 270.0f, colStr, GLUT_BITMAP_HELVETICA_12);

    int timePen = std::min(40, (int)(timeTaken * 1.5f));
    std::string timeStr = "- Time Penalty (R02):       " + std::to_string(timePen) + " pts (" + std::to_string((int)timeTaken) + "s elapsed)";
    drawText(220.0f, 295.0f, timeStr, GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.35f, 0.85f, 1.0f);
    drawText(220.0f, 320.0f, "+ Precision Bonus (R03):   +15 pts", GLUT_BITMAP_HELVETICA_12);

    glColor3f(1.0f, 0.85f, 0.20f);
    std::string totalStr = "TOTAL LEVEL POINTS AWARDED: +" + std::to_string(levelScore) + " PTS";
    drawText(240.0f, 365.0f, totalStr, GLUT_BITMAP_HELVETICA_12);

    if (levelNum < 3) {
        drawButton(190.0f, 385.0f, 130.0f, 36.0f, "NEXT LEVEL", 0.15f, 0.65f, 0.30f);
        drawButton(335.0f, 385.0f, 130.0f, 36.0f, "RESTART (R)", 0.75f, 0.45f, 0.10f);
        drawButton(480.0f, 385.0f, 130.0f, 36.0f, "MAIN MENU", 0.50f, 0.50f, 0.50f);
    } else {
        drawButton(240.0f, 385.0f, 150.0f, 36.0f, "RESTART ALL", 0.15f, 0.65f, 0.30f);
        drawButton(410.0f, 385.0f, 150.0f, 36.0f, "MAIN MENU", 0.50f, 0.50f, 0.50f);
    }
}

// Redesigned Level Fail Popup with Cinema Theme & Specific Cause of Failure
void Renderer::drawLevelFailPopup(int levelNum, const std::string& reason, int collisions, int maxCollisions, float timeTaken) {
    glColor4f(0.04f, 0.02f, 0.02f, 0.85f);
    glRectf(0.0f, 0.0f, 800.0f, 520.0f);

    glColor4f(0.18f, 0.06f, 0.06f, 0.98f);
    glRectf(160.0f, 120.0f, 640.0f, 400.0f);

    glColor3f(0.95f, 0.25f, 0.25f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(160.0f, 120.0f); glVertex2f(640.0f, 120.0f);
        glVertex2f(640.0f, 400.0f); glVertex2f(160.0f, 400.0f);
    glEnd();

    glColor3f(1.0f, 0.30f, 0.30f);
    std::stringstream ssTitle;
    ssTitle << "LEVEL " << levelNum << " FAILED!";
    drawText(295.0f, 165.0f, ssTitle.str(), GLUT_BITMAP_HELVETICA_18);

    // Reason Banner
    glColor4f(0.35f, 0.08f, 0.08f, 0.90f);
    glRectf(190.0f, 190.0f, 610.0f, 245.0f);
    glColor3f(1.0f, 0.40f, 0.40f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(190.0f, 190.0f); glVertex2f(610.0f, 190.0f);
        glVertex2f(610.0f, 245.0f); glVertex2f(190.0f, 245.0f);
    glEnd();

    glColor3f(1.0f, 0.95f, 0.95f);
    drawText(210.0f, 218.0f, "[!] " + reason, GLUT_BITMAP_HELVETICA_12);

    // Stats
    glColor3f(0.85f, 0.85f, 0.90f);
    std::string statStr = "Collisions: " + std::to_string(collisions) + " / " + std::to_string(maxCollisions) +
                          "  |  Time Taken: " + std::to_string((int)timeTaken) + "s / 30s";
    drawText(240.0f, 280.0f, statStr, GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.70f, 0.75f, 0.85f);
    drawText(230.0f, 310.0f, "Tip: Avoid occupied slots & follow the cyan guide trail!", GLUT_BITMAP_HELVETICA_10);

    drawButton(240.0f, 340.0f, 140.0f, 38.0f, "RESTART (R)", 0.75f, 0.45f, 0.10f);
    drawButton(420.0f, 340.0f, 140.0f, 38.0f, "MAIN MENU", 0.50f, 0.50f, 0.50f);
}

void Renderer::drawGameWonPopup(int totalScore, const std::string& playerName) {
    glColor4f(0.02f, 0.05f, 0.08f, 0.85f);
    glRectf(0.0f, 0.0f, 800.0f, 520.0f);

    glColor4f(0.04f, 0.10f, 0.07f, 0.98f);
    glRectf(150.0f, 110.0f, 650.0f, 410.0f);

    glColor3f(1.0f, 0.85f, 0.20f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(150.0f, 110.0f); glVertex2f(650.0f, 110.0f);
        glVertex2f(650.0f, 410.0f); glVertex2f(150.0f, 410.0f);
    glEnd();

    glColor3f(1.0f, 0.85f, 0.20f);
    drawText(230.0f, 160.0f, "ALL 3 LEVELS COMPLETED!", GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.30f, 0.90f, 0.50f);
    std::stringstream ss;
    ss << "Congratulations " << playerName << "! Total Score: " << totalScore << " pts";
    drawText(210.0f, 210.0f, ss.str(), GLUT_BITMAP_HELVETICA_12);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(200.0f, 250.0f, "Your score has been securely saved to the Leaderboard!", GLUT_BITMAP_HELVETICA_12);

    drawButton(240.0f, 310.0f, 140.0f, 40.0f, "PLAY AGAIN", 0.15f, 0.65f, 0.30f);
    drawButton(420.0f, 310.0f, 140.0f, 40.0f, "MAIN MENU", 0.50f, 0.50f, 0.50f);
}

// Mouse Click Hit-Testing
ClickAction Renderer::handleMouseClick(int mouseX, int mouseY, const std::string& gameState,
                                       const PlayerDatabase& db, int& outSelectedPlayerIdx, bool showAuthModal) {
    float mx = (float)mouseX;
    float my = (float)mouseY;

    // 1. Password Prompt Modal Hit-Testing (Priority when open)
    if (showAuthModal) {
        if (mx >= 240.0f && mx <= 390.0f && my >= 295.0f && my <= 327.0f) return ACT_AUTH_CONFIRM;
        if (mx >= 410.0f && mx <= 560.0f && my >= 295.0f && my <= 327.0f) return ACT_AUTH_CANCEL;
        return ACT_NONE;
    }

    // 2. Main Menu
    if (gameState == "MENU") {
        if (mx >= 270.0f && mx <= 530.0f) {
            if (my >= 180.0f && my <= 224.0f) return ACT_MENU_START;
            if (my >= 238.0f && my <= 282.0f) return ACT_MENU_PLAYER_SELECT;
            if (my >= 296.0f && my <= 340.0f) return ACT_MENU_LEADERBOARD;
            if (my >= 354.0f && my <= 398.0f) return ACT_MENU_EXIT;
        }
        return ACT_NONE;
    }

    // 3. Player Selection Screen
    if (gameState == "PLAYER_SELECT") {
        // Field focus clicks
        if (mx >= 190.0f && mx <= 370.0f && my >= 88.0f && my <= 118.0f) return ACT_FIELD_NAME;
        if (mx >= 470.0f && mx <= 590.0f && my >= 88.0f && my <= 118.0f) return ACT_FIELD_PASSWORD;

        // Register Button
        if (mx >= 605.0f && mx <= 700.0f && my >= 88.0f && my <= 118.0f) return ACT_ADD_NEW_PLAYER;

        // Player row action buttons
        int startY = 175;
        const auto& players = db.getPlayers();
        for (size_t i = 0; i < std::min((size_t)5, players.size()); i++) {
            // Play As This Button
            if (mx >= 505.0f && mx <= 615.0f && my >= (float)startY && my <= (float)(startY + 36)) {
                outSelectedPlayerIdx = (int)i;
                return ACT_SELECT_EXISTING_PLAYER;
            }
            // Delete Player Button (User Request)
            if (mx >= 625.0f && mx <= 700.0f && my >= (float)startY && my <= (float)(startY + 36)) {
                outSelectedPlayerIdx = (int)i;
                return ACT_DELETE_PLAYER;
            }
            startY += 45;
        }

        // Back to Menu
        if (mx >= 280.0f && mx <= 520.0f && my >= 440.0f && my <= 476.0f) return ACT_BACK_TO_MENU;
        return ACT_NONE;
    }

    // 4. Leaderboard Screen
    if (gameState == "LEADERBOARD") {
        if (mx >= 280.0f && mx <= 520.0f && my >= 415.0f && my <= 453.0f) return ACT_BACK_TO_MENU;
        return ACT_NONE;
    }

    // 5. Level Win Popup
    if (gameState == "LEVEL_WIN") {
        if (mx >= 190.0f && mx <= 320.0f && my >= 385.0f && my <= 421.0f) return ACT_POPUP_NEXT;
        if (mx >= 335.0f && mx <= 465.0f && my >= 385.0f && my <= 421.0f) return ACT_POPUP_RESTART;
        if (mx >= 480.0f && mx <= 610.0f && my >= 385.0f && my <= 421.0f) return ACT_POPUP_MENU;
        if (mx >= 240.0f && mx <= 390.0f && my >= 385.0f && my <= 421.0f) return ACT_POPUP_RESTART;
        if (mx >= 410.0f && mx <= 560.0f && my >= 385.0f && my <= 421.0f) return ACT_POPUP_MENU;
        return ACT_NONE;
    }

    // 6. Level Fail Popup
    if (gameState == "LEVEL_FAIL") {
        if (mx >= 240.0f && mx <= 380.0f && my >= 340.0f && my <= 378.0f) return ACT_POPUP_RESTART;
        if (mx >= 420.0f && mx <= 560.0f && my >= 340.0f && my <= 378.0f) return ACT_POPUP_MENU;
        return ACT_NONE;
    }

    // 7. Game Won Popup
    if (gameState == "GAME_WON") {
        if (mx >= 240.0f && mx <= 380.0f && my >= 310.0f && my <= 350.0f) return ACT_POPUP_RESTART;
        if (mx >= 420.0f && mx <= 560.0f && my >= 310.0f && my <= 350.0f) return ACT_POPUP_MENU;
        return ACT_NONE;
    }

    // 8. Gameplay HUD Buttons
    if (mx >= 590.0f && mx <= 668.0f && my >= 10.0f && my <= 45.0f) return ACT_HUD_RESTART;
    if (mx >= 676.0f && mx <= 755.0f && my >= 10.0f && my <= 45.0f) return ACT_HUD_EXIT;

    return ACT_NONE;
}
