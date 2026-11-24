#ifndef GAME_H
#define GAME_H

#include "OOPS_Car.h"
#include "OOPS_ParkingLot.h"
#include "COA_Processor.h"
#include "CG_Renderer.h"
#include "PL_DataStructures.h"
#include <string>

enum GameState {
    STATE_MENU = 0,
    STATE_PLAYER_SELECT,
    STATE_LEADERBOARD,
    STATE_PLAYING,
    STATE_LEVEL_WIN,
    STATE_LEVEL_FAIL,
    STATE_GAME_WON
};

class Game {
private:
    GameState currentState;
    int currentLevel;
    float timeRemaining;

    int getMaxCollisionsForLevel(int level) const {
        return 2;
    }

    int currentLevelScore;
    int totalGameScore;
    std::string failReason;

    // Player Management & Security (PL CO4/CO5)
    std::string activePlayerName;
    std::string playerInputBuffer;
    std::string playerPasswordBuffer;
    int activeInputField; // 0 = Name, 1 = Password
    std::string feedbackMessage;

    // Authentication Modal State
    bool showAuthModal;
    std::string authTargetPlayerName;
    std::string authPasswordBuffer;
    std::string authErrorMessage;

    // Alert Banner & Timers
    std::string alertMessage;
    float alertTimer;

    // DSA Data Structures (PL CO3)
    CarQueue waitingQueue;
    ManeuverStack maneuverHistory;
    float maneuverRecordTimer;

    PlayerDatabase playerDB;
    Car playerCar;
    ParkingLot parkingLot;
    COAProcessor coaProcessor;
    Renderer renderer;

    bool keys[256];
    bool specialKeys[256];

public:
    Game(int windowWidth = 800, int windowHeight = 600);
    ~Game();

    void init();
    void update(float dt);
    void render();

    void playFahhSound();

    void startLevel(int level);
    void restartCurrentLevel();
    void nextLevel();
    void goToMenu();

    void onKeyDown(unsigned char key);
    void onKeyUp(unsigned char key);
    void onSpecialKeyDown(int key);
    void onSpecialKeyUp(int key);
    void onMouseClick(int button, int state, int x, int y);
    void onResize(int w, int h);

    std::string getStateString() const;
};

#endif
