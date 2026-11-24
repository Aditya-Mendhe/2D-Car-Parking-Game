#ifndef OOPS_OBSTACLE_H
#define OOPS_OBSTACLE_H

#include "PL_DataStructures.h"
#include <string>
#include <vector>

class Car;

class Obstacle {
private:
    int id;
    float x, y, width, height;
    std::string label;

public:
    Obstacle();
    Obstacle(int _id, float _x, float _y, float _w, float _h, const std::string& _lbl);
    Obstacle(const Obstacle& other);
    ~Obstacle();

    void render() const;

    bool checkCollision(const Vec2D& pt) const;
    bool checkCollision(const std::vector<Vec2D>& corners) const;

    int getId() const { return id; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    std::string getLabel() const { return label; }

    friend bool checkCarOverlap(const Car& car, const Obstacle& obs);
};

#endif
