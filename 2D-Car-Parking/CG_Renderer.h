#ifndef CG_RENDERER_H
#define CG_RENDERER_H

// CG CO3: 2D Primitives & Object Construction (GL_QUADS, GL_TRIANGLES, GL_LINES, GL_LINE_LOOP)
// CG CO4: Coordinate Alignment, Viewport Handling & Structured Rendering Pipeline
// CG CO5: 2D Orthographic Projection & Viewport Boundary Clipping
// CG CO6: Surface Textures, Center Dividers & Color-Blended Headlight Cone

#include "OOPS_Car.h"
#include "OOPS_ParkingLot.h"
#include "COA_Processor.h"
#include "PL_DataStructures.h"
#include <string>

enum ClickAction {
    ACT_NONE = 0,
    ACT_MENU_START,
    ACT_MENU_PLAYER_SELECT,
    ACT_MENU_LEADERBOARD,
    ACT_MENU_EXIT,
    ACT_POPUP_NEXT,
    ACT_POPUP_RESTART,
    ACT_POPUP_MENU,
    ACT_HUD_RESTART,
    ACT_HUD_EXIT,
    ACT_SELECT_EXISTING_PLAYER,
    ACT_DELETE_PLAYER,
    ACT_ADD_NEW_PLAYER,
    ACT_AUTH_CONFIRM,
    ACT_AUTH_CANCEL,
    ACT_FIELD_NAME,
    ACT_FIELD_PASSWORD,
    ACT_BACK_TO_MENU
};

class Renderer {
private:
    int screenWidth;
    int screenHeight;
    float pulseTimer;

public:
    Renderer(int width = 800, int height = 600);
    void initOpenGL();
    void resize(int w, int h);
    void renderScene(Car& car, ParkingLot& lot, COAProcessor& coa,
                     const std::string& gameState, float timeRemainingSec,
                     int currentLevel, int maxCollisions, const std::string& playerName,
                     int lastLevelScore, const std::string& failReason,
                     const std::string& alertMsg, float alertTimer);
    void drawMainMenu(const std::string& activePlayer);
    void drawPlayerSelectScreen(const PlayerDatabase& db, const std::string& nameInput,
                                const std::string& pwdInput, int activeField,
                                const std::string& feedbackMsg, bool showAuthModal,
                                const std::string& authPlayerName, const std::string& authPwdInput,
                                const std::string& authErrorMsg);
    void drawLeaderboardScreen(const PlayerDatabase& db);
    void drawHUD(const Car& car, const ParkingLot& lot, float timeRemainingSec,
                 int currentLevel, int maxCollisions, const std::string& playerName,
                 const std::string& alertMsg, float alertTimer, const std::string& parkingState);
    void drawLevelWinPopup(int levelNum, int levelScore, float timeTaken, int collisions, int maxCollisions);
    void drawLevelFailPopup(int levelNum, const std::string& reason, int collisions, int maxCollisions, float timeTaken);
    void drawGameWonPopup(int totalScore, const std::string& playerName);
    void drawButton(float x, float y, float w, float h, const std::string& label,
                    float r, float g, float b);
    ClickAction handleMouseClick(int mouseX, int mouseY, const std::string& gameState,
                                 const PlayerDatabase& db, int& outSelectedPlayerIdx, bool showAuthModal);
    static void drawText(float x, float y, const std::string& text, void* font = (void*)0x0008);
    void updatePulse(float dt);
};

#endif
