#include "OOPS_Vehicle.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int Vehicle::vehicleCount = 0;

Vehicle::Vehicle()
    : position(0.0f, 0.0f), angle(0.0f), speed(0.0f), width(30.0f), length(58.0f) {
    vehicleCount++;
}

Vehicle::Vehicle(float x, float y, float w, float l, float ang)
    : position(x, y), angle(ang), speed(0.0f), width(w), length(l) {
    vehicleCount++;
}

Vehicle::Vehicle(const Vehicle& other)
    : position(other.position), angle(other.angle), speed(other.speed),
      width(other.width), length(other.length) {
    vehicleCount++;
}

Vehicle::~Vehicle() {
    vehicleCount--;
}

float Vehicle::getAngleDegrees() const {
    return angle * (180.0f / (float)M_PI);
}

int Vehicle::getVehicleCount() {
    return vehicleCount;
}

ParkedCar::ParkedCar()
    : Vehicle(0.0f, 0.0f, 34.0f, 58.0f, 0.0f), registration(""),
      colorR(0.22f), colorG(0.51f), colorB(0.96f) {}

ParkedCar::ParkedCar(float x, float y, const std::string& reg, float r, float g, float b, float w, float l, float ang)
    : Vehicle(x, y, w, l, ang), registration(reg),
      colorR(r), colorG(g), colorB(b) {}

ParkedCar::~ParkedCar() {}

void ParkedCar::update(float dt) {}

void ParkedCar::render() {
    glPushMatrix();
    glTranslatef(position.x, position.y, 0.0f);
    glRotatef(getAngleDegrees(), 0.0f, 0.0f, 1.0f);

    float hl = length / 2.0f;
    float hw = width / 2.0f;

    // 4 Rubber Tires
    glColor3f(0.06f, 0.09f, 0.16f);
    float tw = 12.0f, th = 4.0f;
    glRectf(-hl + 8.0f, -hw - th, -hl + 8.0f + tw, -hw);
    glRectf(hl - 20.0f, -hw - th, hl - 20.0f + tw, -hw);
    glRectf(-hl + 8.0f, hw, -hl + 8.0f + tw, hw + th);
    glRectf(hl - 20.0f, hw, hl - 20.0f + tw, hw + th);

    // Car Body Chassis
    glColor3f(colorR, colorG, colorB);
    glRectf(-hl, -hw, hl, hw);

    // Outline Highlight
    glColor3f(std::min(1.0f, colorR + 0.35f), std::min(1.0f, colorG + 0.35f), std::min(1.0f, colorB + 0.35f));
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-hl, -hw);
        glVertex2f(hl, -hw);
        glVertex2f(hl, hw);
        glVertex2f(-hl, hw);
    glEnd();

    // Windshield & Rear glass (dark tint)
    glColor4f(0.12f, 0.16f, 0.23f, 0.85f);
    glRectf(-hl + 12.0f, -hw + 4.0f, -hl + 22.0f, hw - 4.0f); // Rear window
    glRectf(hl - 22.0f, -hw + 4.0f, hl - 14.0f, hw - 4.0f);   // Front windshield

    // Roof
    glColor3f(colorR * 0.75f, colorG * 0.75f, colorB * 0.75f);
    glRectf(-hl + 23.0f, -hw + 5.0f, hl - 23.0f, hw - 5.0f);

    // Headlights (facing +X forward)
    glColor3f(1.0f, 0.95f, 0.55f);
    glRectf(hl - 4.0f, -hw + 3.0f, hl, -hw + 8.0f);
    glRectf(hl - 4.0f, hw - 8.0f, hl, hw - 3.0f);

    // Taillights (facing -X rear)
    glColor3f(0.95f, 0.15f, 0.15f);
    glRectf(-hl, -hw + 3.0f, -hl + 4.0f, -hw + 8.0f);
    glRectf(-hl, hw - 8.0f, -hl + 4.0f, hw - 3.0f);

    glPopMatrix();
}
