#include "Game.h"
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <windows.h>
#include <mmsystem.h>

Game::Game(int windowWidth, int windowHeight)
    : currentState(STATE_MENU), currentLevel(1), timeRemaining(30.0f),
      currentLevelScore(0), totalGameScore(0), failReason(""),
      activePlayerName("Player 1"), playerInputBuffer(""), playerPasswordBuffer(""),
      activeInputField(0), feedbackMessage(""),
      showAuthModal(false), authTargetPlayerName(""), authPasswordBuffer(""), authErrorMessage(""),
      alertMessage(""), alertTimer(0.0f), maneuverRecordTimer(0.0f),
      playerDB("players.txt"),
      playerCar(95.0f, 160.0f, 0.0f),
      parkingLot(windowWidth, windowHeight),
      renderer(windowWidth, windowHeight) {
    for (int i = 0; i < 256; i++) {
        keys[i] = false;
        specialKeys[i] = false;
    }
}

Game::~Game() {
    mciSendStringA("close fahh_snd", NULL, 0, NULL);
}

void Game::playFahhSound() {
    // Zero-latency playback: alias is pre-opened; rewind and play instantly
    mciSendStringA("play fahh_snd from 0", NULL, 0, NULL);
}

void Game::init() {
    renderer.initOpenGL();

    // Pre-open audio alias once at launch for zero-delay instant playback
    mciSendStringA("close fahh_snd", NULL, 0, NULL);
    mciSendStringA("open \"fahh.mp3\" type mpegvideo alias fahh_snd", NULL, 0, NULL);

    playerDB.loadFromFile();
    if (!playerDB.getPlayers().empty()) {
        activePlayerName = playerDB.getPlayers()[0].name;
    }

    // Initialize PL CO3 CarQueue with vehicles waiting at the entrance gate
    waitingQueue.clear();
    waitingQueue.enqueue("MH12-DE-1001", "Sedan");
    waitingQueue.enqueue("DL04-CA-2045", "SUV");
    waitingQueue.enqueue("KA01-MJ-9999", "Hatchback");

    currentState = STATE_MENU;
    currentLevel = 1;
    totalGameScore = 0;
}

std::string Game::getStateString() const {
    switch (currentState) {
        case STATE_MENU:          return "MENU";
        case STATE_PLAYER_SELECT: return "PLAYER_SELECT";
        case STATE_LEADERBOARD:   return "LEADERBOARD";
        case STATE_PLAYING:       return "PLAYING";
        case STATE_LEVEL_WIN:     return "LEVEL_WIN";
        case STATE_LEVEL_FAIL:    return "LEVEL_FAIL";
        case STATE_GAME_WON:      return "GAME_WON";
        default:                  return "UNKNOWN";
    }
}

void Game::startLevel(int level) {
    currentLevel = level;
    if (level == 1) timeRemaining = 30.0f;
    else if (level == 2) timeRemaining = 28.0f;
    else timeRemaining = 26.0f;

    parkingLot.loadLevel(level);

    alertMessage = "";
    alertTimer = 0.0f;
    maneuverHistory.clear();
    maneuverRecordTimer = 0.0f;

    if (level == 1) {
        playerCar.reset(95.0f, 160.0f, 0.0f);
    } else if (level == 2) {
        playerCar.reset(90.0f, 460.0f, 0.0f);
    } else {
        playerCar.reset(120.0f, 135.0f, 1.5707963f);
    }

    coaProcessor.reset();
    currentState = STATE_PLAYING;
    std::cout << "[GAME] Level " << level << " started. Player: " << activePlayerName << "\n";
}

void Game::restartCurrentLevel() {
    startLevel(currentLevel);
}

void Game::nextLevel() {
    if (currentLevel < 3) {
        startLevel(currentLevel + 1);
    } else {
        currentState = STATE_GAME_WON;
    }
}

void Game::goToMenu() {
    currentState = STATE_MENU;
}

void Game::update(float dt) {
    renderer.updatePulse(dt);

    if (alertTimer > 0.0f) {
        alertTimer -= dt;
        if (alertTimer <= 0.0f) {
            alertMessage = "";
        }
    }

    if (currentState != STATE_PLAYING) return;

    timeRemaining -= dt;
    if (timeRemaining <= 0.0f) {
        timeRemaining = 0.0f;
        failReason = "Time Expired!";
        playFahhSound();
        currentState = STATE_LEVEL_FAIL;
        return;
    }

    bool forward  = keys['w'] || keys['W'] || specialKeys[GLUT_KEY_UP];
    bool backward = keys['s'] || keys['S'] || specialKeys[GLUT_KEY_DOWN];
    bool left     = keys['a'] || keys['A'] || specialKeys[GLUT_KEY_LEFT];
    bool right    = keys['d'] || keys['D'] || specialKeys[GLUT_KEY_RIGHT];
    bool brake    = keys[' '];

    playerCar.handleInput(forward, backward, left, right, brake);
    playerCar.update(dt);

    // Record maneuver path in stack (PL CO3) every 0.3s
    maneuverRecordTimer += dt;
    if (maneuverRecordTimer >= 0.30f) {
        maneuverHistory.push(playerCar.getPosition(), playerCar.getAngle());
        maneuverRecordTimer = 0.0f;
    }

    // Solid collision detection with obstacles and occupied parked cars
    std::string hitEntity;
    bool isOccupiedCar = false;
    auto corners = playerCar.getCorners();
    if (parkingLot.checkCollision(corners, playerCar.getPosition(), hitEntity, isOccupiedCar)) {
        playerCar.registerCollision(isOccupiedCar);
        std::cout << "[COLLISION] " << hitEntity << " | isCar=" << isOccupiedCar
                  << " | hits=" << playerCar.getCollisions() << "\n";

        if (isOccupiedCar) {
            alertMessage = "Car slot is occupied!";
            alertTimer = 2.5f;
            playerCar.pushBack(12.0f);
        }

        // Sound effect on collision
        playFahhSound();

        int maxAllowed = getMaxCollisionsForLevel(currentLevel);
        if (playerCar.getCollisions() >= maxAllowed) {
            failReason = "Max Collisions Exceeded! (" + std::to_string(maxAllowed) + " / " + std::to_string(maxAllowed) + " hits)";
            playFahhSound();
            currentState = STATE_LEVEL_FAIL;
            return;
        }
    }

    ParkingSlot* target = parkingLot.getTargetSlot();
    coaProcessor.executeParkingCheck(playerCar.getPosition().x, playerCar.getPosition().y,
                                    playerCar.getAngleDegrees(), target);

    if (coaProcessor.getDecisionState() == "PARK_SUCCESS_CONFIRMED" &&
        std::abs(playerCar.getSpeed()) < 0.60f && !playerCar.getIsParked()) {
        playerCar.setParked(true);

        float initialTime = (currentLevel == 1) ? 30.0f : ((currentLevel == 2) ? 28.0f : 26.0f);
        float timeTaken = initialTime - timeRemaining;
        currentLevelScore = coaProcessor.calculateScore(playerCar.getCollisions(), timeTaken);
        totalGameScore += currentLevelScore;
        playerCar.setScore(totalGameScore);

        playerDB.updatePlayerScore(activePlayerName, currentLevelScore, currentLevel,
                                   playerCar.getCollisions(), timeTaken);

        if (currentLevel < 3) {
            currentState = STATE_LEVEL_WIN;
            std::cout << "[SUCCESS] Level " << currentLevel << " Score: " << currentLevelScore << "\n";
        } else {
            currentState = STATE_GAME_WON;
            std::cout << "[WIN] All levels done! Total: " << totalGameScore << "\n";
        }
    }
}

void Game::render() {
    if (currentState == STATE_PLAYER_SELECT) {
        renderer.drawPlayerSelectScreen(playerDB, playerInputBuffer, playerPasswordBuffer,
                                       activeInputField, feedbackMessage, showAuthModal,
                                       authTargetPlayerName, authPasswordBuffer, authErrorMessage);
    } else if (currentState == STATE_LEADERBOARD) {
        renderer.drawLeaderboardScreen(playerDB);
    } else {
        renderer.renderScene(playerCar, parkingLot, coaProcessor, getStateString(),
                             timeRemaining, currentLevel, getMaxCollisionsForLevel(currentLevel),
                             activePlayerName, currentLevelScore, failReason,
                             alertMessage, alertTimer);
    }
}

void Game::onMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int selectedPlayerIdx = -1;
        ClickAction act = renderer.handleMouseClick(x, y, getStateString(), playerDB, selectedPlayerIdx, showAuthModal);

        if (act == ACT_MENU_START) {
            totalGameScore = 0;
            startLevel(1);
        } else if (act == ACT_MENU_PLAYER_SELECT) {
            playerInputBuffer = "";
            playerPasswordBuffer = "";
            activeInputField = 0;
            feedbackMessage = "";
            showAuthModal = false;
            currentState = STATE_PLAYER_SELECT;
        } else if (act == ACT_MENU_LEADERBOARD) {
            playerDB.sortLeaderboard();
            currentState = STATE_LEADERBOARD;
        } else if (act == ACT_MENU_EXIT) {
            std::exit(0);
        } else if (act == ACT_POPUP_NEXT) {
            nextLevel();
        } else if (act == ACT_POPUP_RESTART || act == ACT_HUD_RESTART) {
            restartCurrentLevel();
        } else if (act == ACT_POPUP_MENU || act == ACT_HUD_EXIT || act == ACT_BACK_TO_MENU) {
            showAuthModal = false;
            goToMenu();
        } else if (act == ACT_FIELD_NAME) {
            activeInputField = 0;
        } else if (act == ACT_FIELD_PASSWORD) {
            activeInputField = 1;
        } else if (act == ACT_ADD_NEW_PLAYER) {
            if (playerInputBuffer.empty()) {
                feedbackMessage = "Username cannot be empty!";
            } else if (playerPasswordBuffer.empty()) {
                feedbackMessage = "Password cannot be empty!";
            } else {
                bool ok = playerDB.registerPlayer(playerInputBuffer, playerPasswordBuffer);
                if (ok) {
                    activePlayerName = playerInputBuffer;
                    playerInputBuffer = "";
                    playerPasswordBuffer = "";
                    feedbackMessage = "Account registered successfully!";
                    goToMenu();
                } else {
                    feedbackMessage = "Player username already exists!";
                }
            }
        } else if (act == ACT_SELECT_EXISTING_PLAYER) {
            const auto& players = playerDB.getPlayers();
            if (selectedPlayerIdx >= 0 && selectedPlayerIdx < (int)players.size()) {
                authTargetPlayerName = players[selectedPlayerIdx].name;
                authPasswordBuffer = "";
                authErrorMessage = "";
                showAuthModal = true;
            }
        } else if (act == ACT_DELETE_PLAYER) {
            const auto& players = playerDB.getPlayers();
            if (selectedPlayerIdx >= 0 && selectedPlayerIdx < (int)players.size()) {
                std::string target = players[selectedPlayerIdx].name;
                playerDB.deletePlayer(target);
                feedbackMessage = "Deleted player: " + target;
                if (activePlayerName == target) {
                    if (!playerDB.getPlayers().empty()) {
                        activePlayerName = playerDB.getPlayers()[0].name;
                    } else {
                        activePlayerName = "Guest";
                    }
                }
            }
        } else if (act == ACT_AUTH_CONFIRM) {
            if (playerDB.authenticate(authTargetPlayerName, authPasswordBuffer)) {
                activePlayerName = authTargetPlayerName;
                showAuthModal = false;
                authPasswordBuffer = "";
                authErrorMessage = "";
                goToMenu();
            } else {
                authErrorMessage = "Incorrect Password! Access Denied.";
            }
        } else if (act == ACT_AUTH_CANCEL) {
            showAuthModal = false;
            authPasswordBuffer = "";
            authErrorMessage = "";
        }
    }
}

void Game::onKeyDown(unsigned char key) {
    // Password Modal key handling
    if (showAuthModal) {
        if (key == 13) { // Enter to authenticate
            if (playerDB.authenticate(authTargetPlayerName, authPasswordBuffer)) {
                activePlayerName = authTargetPlayerName;
                showAuthModal = false;
                authPasswordBuffer = "";
                authErrorMessage = "";
                goToMenu();
            } else {
                authErrorMessage = "Incorrect Password! Access Denied.";
            }
        } else if (key == 27) { // Escape to cancel modal
            showAuthModal = false;
            authPasswordBuffer = "";
            authErrorMessage = "";
        } else if (key == 8) { // Backspace
            if (!authPasswordBuffer.empty()) {
                authPasswordBuffer.pop_back();
            }
        } else if (key >= 32 && key <= 126 && authPasswordBuffer.length() < 16) {
            authPasswordBuffer.push_back((char)key);
        }
        return;
    }

    // Player Selection screen key handling
    if (currentState == STATE_PLAYER_SELECT) {
        if (key == 9) { // Tab key switches between Name and Password fields
            activeInputField = 1 - activeInputField;
        } else if (key == 13) { // Enter registers new player
            if (playerInputBuffer.empty()) {
                feedbackMessage = "Username cannot be empty!";
            } else if (playerPasswordBuffer.empty()) {
                feedbackMessage = "Password cannot be empty!";
            } else {
                bool ok = playerDB.registerPlayer(playerInputBuffer, playerPasswordBuffer);
                if (ok) {
                    activePlayerName = playerInputBuffer;
                    playerInputBuffer = "";
                    playerPasswordBuffer = "";
                    goToMenu();
                } else {
                    feedbackMessage = "Player username already exists!";
                }
            }
        } else if (key == 8) { // Backspace
            if (activeInputField == 0 && !playerInputBuffer.empty()) {
                playerInputBuffer.pop_back();
            } else if (activeInputField == 1 && !playerPasswordBuffer.empty()) {
                playerPasswordBuffer.pop_back();
            }
        } else if (key == 27) {
            goToMenu();
        } else if (key >= 32 && key <= 126) {
            if (activeInputField == 0 && playerInputBuffer.length() < 16) {
                playerInputBuffer.push_back((char)key);
            } else if (activeInputField == 1 && playerPasswordBuffer.length() < 16) {
                playerPasswordBuffer.push_back((char)key);
            }
        }
        return;
    }

    if (currentState == STATE_LEADERBOARD) {
        if (key == 27 || key == ' ' || key == 13) {
            goToMenu();
        }
        return;
    }

    if (currentState == STATE_MENU) {
        if (key == ' ' || key == 13) {
            totalGameScore = 0;
            startLevel(1);
        } else if (key == 27) {
            std::exit(0);
        }
        return;
    }

    keys[key] = true;

    if (key == 27) {
        goToMenu();
    } else if (key == 'r' || key == 'R') {
        restartCurrentLevel();
    } else if (key == 't' || key == 'T') {
        parkingLot.switchTargetSlot();
    } else if (key == 'p' || key == 'P') {
        parkingLot.getSlotManager().display();
        parkingLot.getDynamicObstacles().displayList();
    } else if (key == 'u' || key == 'U') {
        // PL CO3: Undo last maneuver using ManeuverStack
        Vec2D prevPos;
        float prevAng;
        if (maneuverHistory.pop(prevPos, prevAng)) {
            playerCar.setPosition(prevPos);
            playerCar.setAngle(prevAng);
            playerCar.setSpeed(0.0f);
            std::cout << "[STACK] Maneuver undone. Position restored to (" << prevPos.x << ", " << prevPos.y << ")\n";
        }
    }
}

void Game::onKeyUp(unsigned char key) {
    keys[key] = false;
}

void Game::onSpecialKeyDown(int key) {
    if (key >= 0 && key < 256) {
        specialKeys[key] = true;
    }
}

void Game::onSpecialKeyUp(int key) {
    if (key >= 0 && key < 256) {
        specialKeys[key] = false;
    }
}

void Game::onResize(int w, int h) {
    renderer.resize(w, h);
}
