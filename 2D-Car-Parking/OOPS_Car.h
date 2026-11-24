#ifndef OOPS_CAR_H
#define OOPS_CAR_H

#include "OOPS_Vehicle.h"
#include <vector>

class Obstacle;

class Car : public Vehicle {
private:
    float steerAngle;
    float maxSpeed;
    float maxReverseSpeed;
    float acceleration;
    float reverseAcceleration;
    float friction;
    float turnRate;
    int score;
    int collisions;
    bool isParked;
    bool isColliding;
    int collisionCooldown;

public:
    Car();
    Car(float x, float y, float ang);
    Car(const Car& other);
    ~Car() override;

    void update(float dt) override;
    void render() override;

    void handleInput(bool forward, bool backward, bool left, bool right, bool brake);
    void registerCollision(bool isHardSolid = false);
    void pushBack(float amount);

    void reset();
    void reset(float x, float y, float ang);

    void setParked(bool state);
    void setScore(int s) { score = s; }

    std::vector<Vec2D> getCorners() const;
    int getCollisions() const { return collisions; }
    int getScore() const { return score; }
    bool getIsParked() const { return isParked; }
    bool getIsColliding() const { return isColliding; }

    friend bool checkCarOverlap(const Car& car, const Obstacle& obs);
};

#endif
