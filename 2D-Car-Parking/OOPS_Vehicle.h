#ifndef OOPS_VEHICLE_H
#define OOPS_VEHICLE_H

#include "PL_DataStructures.h"
#include <string>

class Vehicle {
protected:
    Vec2D position;
    float angle;
    float speed;
    float width;
    float length;
    static int vehicleCount;

public:
    Vehicle();
    Vehicle(float x, float y, float w, float l, float ang);
    Vehicle(const Vehicle& other);
    virtual ~Vehicle();

    virtual void update(float dt) = 0;
    virtual void render() = 0;

    Vec2D getPosition() const { return position; }
    float getAngle() const { return angle; }
    float getSpeed() const { return speed; }
    float getWidth() const { return width; }
    float getLength() const { return length; }
    float getAngleDegrees() const;

    void setPosition(const Vec2D& p) { position = p; }
    void setPosition(float x, float y) { position.x = x; position.y = y; }
    void setAngle(float a) { angle = a; }
    void setSpeed(float s) { speed = s; }

    static int getVehicleCount();
};

class ParkedCar : public Vehicle {
private:
    std::string registration;
    float colorR, colorG, colorB;

public:
    ParkedCar();
    ParkedCar(float x, float y, const std::string& reg, float r = 0.22f, float g = 0.51f, float b = 0.96f,
              float w = 34.0f, float l = 58.0f, float ang = 0.0f);
    ~ParkedCar();

    void update(float dt) override;
    void render() override;
    std::string getRegistration() const { return registration; }
};

#endif
