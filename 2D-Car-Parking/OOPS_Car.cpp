#include "OOPS_Car.h"
#include "OOPS_Obstacle.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Car::Car()
    : Vehicle(120.0f, 420.0f, 30.0f, 58.0f, -1.5707963f),
      steerAngle(0.0f), maxSpeed(3.0f), maxReverseSpeed(-1.6f),
      acceleration(0.10f), reverseAcceleration(0.07f), friction(0.95f), turnRate(0.042f),
      score(1000), collisions(0), isParked(false), isColliding(false), collisionCooldown(0) {}

Car::Car(float x, float y, float ang)
    : Vehicle(x, y, 30.0f, 58.0f, ang),
      steerAngle(0.0f), maxSpeed(3.0f), maxReverseSpeed(-1.6f),
      acceleration(0.10f), reverseAcceleration(0.07f), friction(0.95f), turnRate(0.042f),
      score(1000), collisions(0), isParked(false), isColliding(false), collisionCooldown(0) {}

Car::Car(const Car& other)
    : Vehicle(other),
      steerAngle(other.steerAngle), maxSpeed(other.maxSpeed),
      maxReverseSpeed(other.maxReverseSpeed), acceleration(other.acceleration),
      reverseAcceleration(other.reverseAcceleration), friction(other.friction),
      turnRate(other.turnRate), score(other.score), collisions(other.collisions),
      isParked(other.isParked), isColliding(other.isColliding),
      collisionCooldown(other.collisionCooldown) {}

Car::~Car() {}

void Car::reset() {
    reset(120.0f, 420.0f, -1.5707963f);
}

void Car::reset(float x, float y, float ang) {
    position = Vec2D(x, y);
    angle = ang;
    speed = 0.0f;
    steerAngle = 0.0f;
    score = 1000;
    collisions = 0;
    isParked = false;
    isColliding = false;
    collisionCooldown = 0;
}

void Car::handleInput(bool forward, bool backward, bool left, bool right, bool brake) {
    if (isParked) return;

    if (forward) {
        speed += acceleration;
        if (speed > maxSpeed) speed = maxSpeed;
    } else if (backward) {
        speed -= reverseAcceleration;
        if (speed < maxReverseSpeed) speed = maxReverseSpeed;
    } else {
        speed *= friction;
        if (std::abs(speed) < 0.015f) speed = 0.0f;
    }

    if (brake) {
        speed *= 0.80f;
    }

    float steerDir = 0.0f;
    if (left) steerDir -= 1.0f;
    if (right) steerDir += 1.0f;

    if (steerDir != 0.0f) {
        float speedFactor = std::min(1.0f, std::abs(speed) / 1.2f);
        float dir = (speed >= 0.0f) ? 1.0f : -1.0f;
        angle += steerDir * turnRate * speedFactor * dir;
    }
}

void Car::update(float dt) {
    if (isParked) return;

    position.x += speed * std::cos(angle);
    position.y += speed * std::sin(angle);

    if (collisionCooldown > 0) {
        collisionCooldown--;
        if (collisionCooldown <= 30) {
            isColliding = false;
        }
    }
}

// CG CO3: 2D object construction and rotated bounding box algorithm
std::vector<Vec2D> Car::getCorners() const {
    float hw = width / 2.0f;
    float hl = length / 2.0f;
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);

    Vec2D localCorners[4] = {
        Vec2D(hl, -hw),
        Vec2D(hl, hw),
        Vec2D(-hl, hw),
        Vec2D(-hl, -hw)
    };

    std::vector<Vec2D> worldCorners;
    for (int i = 0; i < 4; i++) {
        worldCorners.push_back(Vec2D(
            position.x + (localCorners[i].x * cosA - localCorners[i].y * sinA),
            position.y + (localCorners[i].x * sinA + localCorners[i].y * cosA)
        ));
    }
    return worldCorners;
}

void Car::pushBack(float amount) {
    float pushDir = (speed >= 0.0f) ? -1.0f : 1.0f;
    position.x += std::cos(angle) * pushDir * amount;
    position.y += std::sin(angle) * pushDir * amount;
    speed = 0.0f;
}

void Car::registerCollision(bool isHardSolid) {
    if (collisionCooldown == 0) {
        collisions++;
        isColliding = true;
        score = std::max(0, score - 200);
        collisionCooldown = 45;

        float pushDist = isHardSolid ? 18.0f : 10.0f;
        float pushDir = (speed >= 0.0f) ? -1.0f : 1.0f;
        position.x += std::cos(angle) * pushDir * pushDist;
        position.y += std::sin(angle) * pushDir * pushDist;
        speed = isHardSolid ? 0.0f : (-speed * 0.25f);
    } else if (isHardSolid) {
        // Enforce solid barrier during cooldown to prevent sliding through
        pushBack(4.0f);
    }
}

void Car::setParked(bool state) {
    isParked = state;
    if (state) speed = 0.0f;
}

// CG CO3 & CO6: Car construction and color-blended headlight lighting effect
void Car::render() {
    glPushMatrix();
    glTranslatef(position.x, position.y, 0.0f);
    glRotatef(getAngleDegrees(), 0.0f, 0.0f, 1.0f);

    float hw = width / 2.0f;
    float hl = length / 2.0f;

    // CG CO6: Headlight Effect with per-vertex color blending (light cream to yellow)
    glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 1.0f, 0.80f, 0.40f); // Light cream apex at headlights
        glVertex2f(hl, 0.0f);
        glColor4f(1.0f, 1.0f, 0.20f, 0.05f); // Amber yellow base spread
        glVertex2f(hl + 85.0f, -hw - 24.0f);
        glVertex2f(hl + 85.0f, hw + 24.0f);
    glEnd();

    // Wheels (4 rubber tires)
    glColor3f(0.08f, 0.09f, 0.12f);
    float wheelW = 14.0f, wheelH = 6.0f;
    glRectf(hl - 18.0f, -hw - 3.0f, hl - 18.0f + wheelW, -hw + wheelH - 3.0f);
    glRectf(hl - 18.0f, hw - wheelH + 3.0f, hl - 18.0f + wheelW, hw + 3.0f);
    glRectf(-hl + 6.0f, -hw - 3.0f, -hl + 6.0f + wheelW, -hw + wheelH - 3.0f);
    glRectf(-hl + 6.0f, hw - wheelH + 3.0f, -hl + 6.0f + wheelW, hw + 3.0f);

    // Car Body chassis
    if (isColliding) {
        glColor3f(0.95f, 0.18f, 0.18f); // Red collision flash
    } else {
        glColor3f(0.15f, 0.48f, 0.95f); // Vibrant blue body
    }
    glBegin(GL_QUADS);
        glVertex2f(-hl, -hw);
        glVertex2f(hl, -hw);
        glVertex2f(hl, hw);
        glVertex2f(-hl, hw);
    glEnd();

    // Body Outline
    glColor3f(0.10f, 0.25f, 0.65f);
    glLineWidth(1.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-hl, -hw);
        glVertex2f(hl, -hw);
        glVertex2f(hl, hw);
        glVertex2f(-hl, hw);
    glEnd();

    // Cabin Roof
    glColor3f(0.10f, 0.14f, 0.22f);
    glRectf(-hl + 12.0f, -hw + 4.0f, hl - 12.0f, hw - 4.0f);

    // Front and Rear Windshields
    glColor3f(0.35f, 0.80f, 1.0f);
    glRectf(hl - 22.0f, -hw + 5.0f, hl - 15.0f, hw - 5.0f);
    glRectf(-hl + 14.0f, -hw + 5.0f, -hl + 19.0f, hw - 5.0f);

    // Front Directional Arrow
    glColor3f(1.0f, 0.95f, 0.20f);
    glBegin(GL_TRIANGLES);
        glVertex2f(hl - 3.0f, 0.0f);
        glVertex2f(hl - 12.0f, -6.0f);
        glVertex2f(hl - 12.0f, 6.0f);
    glEnd();

    // Headlight bulbs (Cream white) & Taillights (Red)
    glColor3f(1.0f, 1.0f, 0.9f);
    glRectf(hl - 2.0f, -hw + 2.0f, hl + 1.0f, -hw + 7.0f);
    glRectf(hl - 2.0f, hw - 7.0f, hl + 1.0f, hw - 2.0f);

    glColor3f(0.95f, 0.10f, 0.10f);
    glRectf(-hl - 1.0f, -hw + 2.0f, -hl + 2.0f, -hw + 7.0f);
    glRectf(-hl - 1.0f, hw - 7.0f, -hl + 2.0f, hw - 2.0f);

    glPopMatrix();
}

bool checkCarOverlap(const Car& car, const Obstacle& obs) {
    return obs.checkCollision(car.getCorners());
}
