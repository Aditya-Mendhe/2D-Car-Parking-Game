#include "Game.h"
#include <GL/glut.h>
#include <iostream>

static Game g_game(800, 600);

static void displayCallback() {
    g_game.render();
    glutSwapBuffers();
}

static void timerCallback(int value) {
    g_game.update(1.0f / 60.0f);
    glutPostRedisplay();
    glutTimerFunc(16, timerCallback, 0);
}

static void reshapeCallback(int w, int h) {
    g_game.onResize(w, h);
}

static void keyboardDownCallback(unsigned char key, int x, int y) {
    g_game.onKeyDown(key);
}

static void keyboardUpCallback(unsigned char key, int x, int y) {
    g_game.onKeyUp(key);
}

static void specialKeyDownCallback(int key, int x, int y) {
    g_game.onSpecialKeyDown(key);
}

static void specialKeyUpCallback(int key, int x, int y) {
    g_game.onSpecialKeyUp(key);
}

static void mouseCallback(int button, int state, int x, int y) {
    g_game.onMouseClick(button, state, x, y);
}

int main(int argc, char** argv) {
    std::cout << "========================================================\n";
    std::cout << "  2D CAR PARKING GAME (SEMESTER III INTEGRATED PROJECT)\n";
    std::cout << "  MIT Academy of Engineering | Dept. of Computer Engg.\n";
    std::cout << "========================================================\n";
    std::cout << "  [COA] Native Assembly File: COA_Processor.asm (NASM)\n";
    std::cout << "  [OOP] Vehicle Base -> Car & ParkedCar Derived Classes\n";
    std::cout << "  [DSA] Slot Array Manager & Dynamic Obstacle Linked List\n";
    std::cout << "  [CG]  OpenGL 2D Primitives, Transformations & Viewport\n";
    std::cout << "========================================================\n\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(80, 30);
    glutCreateWindow("2D Car Parking Game - Semester III");

    g_game.init();

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardDownCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialKeyDownCallback);
    glutSpecialUpFunc(specialKeyUpCallback);
    glutMouseFunc(mouseCallback);
    glutTimerFunc(16, timerCallback, 0);

    glutMainLoop();
    return 0;
}
