#ifndef OOPS_PARKING_LOT_H
#define OOPS_PARKING_LOT_H

#include "PL_DataStructures.h"
#include "OOPS_Obstacle.h"
#include "OOPS_Vehicle.h"
#include <vector>

class ParkingLot {
private:
    SlotArrayManager slotManager;
    DynamicObstacleList dynamicObstacles;
    std::vector<Obstacle> staticObstacles;
    std::vector<ParkedCar> parkedVehicles;
    std::vector<Vec2D> guidePath;

    float minX, maxX, minY, maxY;
    int currentLevel;

public:
    ParkingLot(int windowWidth = 800, int windowHeight = 600);
    ~ParkingLot();

    void loadLevel(int level);
    void render(float pulseScale = 1.0f);

    bool checkCollision(const std::vector<Vec2D>& carCorners, const Vec2D& carCenter,
                        std::string& outCollisionEntity, bool& outIsOccupiedCar);

    SlotArrayManager& getSlotManager() { return slotManager; }
    const SlotArrayManager& getSlotManager() const { return slotManager; }
    DynamicObstacleList& getDynamicObstacles() { return dynamicObstacles; }
    ParkingSlot* getTargetSlot() { return slotManager.getTargetSlot(); }
    const std::vector<Vec2D>& getGuidePath() const { return guidePath; }
    int getCurrentLevel() const { return currentLevel; }

    void switchTargetSlot();
};

#endif
