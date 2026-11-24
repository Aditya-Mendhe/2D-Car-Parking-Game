#ifndef PL_DATA_STRUCTURES_H
#define PL_DATA_STRUCTURES_H

// ==============================================================================
// Subject: Programming Lab (2304214) & Problem Solving Using OOP (2310261L)
// PL CO1: Contiguous Array Storage (SlotArrayManager) with Insert, Delete & Search
// PL CO2: Singly Linked List (DynamicObstacleList) with Pointer Manipulation
// PL CO3: FIFO Queue (CarQueue) for Entry Gate & LIFO Stack (ManeuverStack) for Undo
// PL CO4 & CO5: Hash Table with Chaining (PlayerHashTable), Salted Passwords & Sorting
// OOP CO3: Exception Hierarchy (ParkingException, InvalidInput, SlotConflict, LotFull)
// OOP CO4: Smart Slot Decision Rules & Tiered Fee Calculation Logic
// ==============================================================================

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <cctype>

// ------------------------------------------------------------------------------
// OOP CO3: Custom Exception Hierarchy
// ------------------------------------------------------------------------------
class ParkingException : public std::runtime_error {
public:
    ParkingException(const std::string& m) : std::runtime_error(m) {}
};

class InvalidInputException : public ParkingException {
public:
    InvalidInputException(const std::string& m) : ParkingException("Invalid input - " + m) {}
};

class SlotConflictException : public ParkingException {
public:
    SlotConflictException(const std::string& m) : ParkingException("Slot conflict - " + m) {}
};

class LotFullException : public ParkingException {
public:
    LotFullException(const std::string& m) : ParkingException("Lot full - " + m) {}
};

bool validPlate(const std::string& n);

// ------------------------------------------------------------------------------
// Vector 2D Geometry
// ------------------------------------------------------------------------------
struct Vec2D {
    float x;
    float y;

    Vec2D(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
    Vec2D operator+(const Vec2D& o) const { return Vec2D(x + o.x, y + o.y); }
    Vec2D operator-(const Vec2D& o) const { return Vec2D(x - o.x, y - o.y); }
    Vec2D operator*(float s) const { return Vec2D(x * s, y * s); }
    bool operator==(const Vec2D& o) const { return x == o.x && y == o.y; }
    float length() const { return std::sqrt(x * x + y * y); }
};

// ------------------------------------------------------------------------------
// OOP CO4: Slot Type Classification
// ------------------------------------------------------------------------------
enum SlotType {
    SLOT_COMPACT = 0,
    SLOT_LARGE = 1
};

class ParkingSlot {
public:
    std::string slotID;
    float x, y;
    float width, height;
    float targetAngle;
    bool isOccupied;
    bool isTarget;
    std::string vehicleReg;
    SlotType slotType;

    ParkingSlot();
    ParkingSlot(std::string id, float _x, float _y, float w = 95.0f, float h = 50.0f,
                float angle = 0.0f, bool occ = false, std::string reg = "", SlotType type = SLOT_COMPACT);

    bool isParked(float carX, float carY, float carAngleDeg, float tolPos, float tolAngle) const;
};

const int MAX_SLOTS = 8;


class SlotArrayManager {
private:
    ParkingSlot slots[MAX_SLOTS];
    int count;

public:
    SlotArrayManager();
    void clear();
    bool insert(const ParkingSlot& slot);
    bool deleteByIndex(int index);
    int linearSearch(const std::string& id) const;
    int binarySearch(const std::string& id) const;
    bool updateOccupancy(int index, bool occ, const std::string& reg = "");
    void display() const;
    void setTarget(const std::string& id);
    ParkingSlot* getTargetSlot();
    int getCount() const { return count; }
    ParkingSlot& getSlot(int i) { return slots[i]; }
    const ParkingSlot& getSlot(int i) const { return slots[i]; }
};

// OOP CO4 Decision Functions
int chooseBestSlot(bool isLargeCar, int wantedSlotNum, SlotArrayManager& slotMgr);
int calculateParkingFee(bool isLargeCar, int hours);

// ------------------------------------------------------------------------------
// PL CO2: Singly Linked List Dynamic Obstacles
// ------------------------------------------------------------------------------
struct DynamicObstacleNode {
    int id;
    float x, y, width, height;
    std::string label;
    DynamicObstacleNode* next;

    DynamicObstacleNode(int _id, float _x, float _y, float _w, float _h, const std::string& _lbl)
        : id(_id), x(_x), y(_y), width(_w), height(_h), label(_lbl), next(nullptr) {}
};

class DynamicObstacleList {
private:
    DynamicObstacleNode* head;
    int nodeCount;

public:
    DynamicObstacleList();
    ~DynamicObstacleList();
    void insertFront(int id, float x, float y, float w, float h, const std::string& label);
    void insertBack(int id, float x, float y, float w, float h, const std::string& label);
    bool deleteByID(int id);
    void clear();
    void displayList() const;
    DynamicObstacleNode* getHead() const { return head; }
    int getCount() const { return nodeCount; }
};

// ------------------------------------------------------------------------------
// PL CO3: FIFO Waiting Entry Queue
// ------------------------------------------------------------------------------
struct QueuedCar {
    std::string plate;
    std::string model;
    QueuedCar* next;
    QueuedCar(const std::string& p, const std::string& m) : plate(p), model(m), next(nullptr) {}
};

class CarQueue {
private:
    QueuedCar* frontNode;
    QueuedCar* rearNode;
    int count;

public:
    CarQueue();
    ~CarQueue();
    void enqueue(const std::string& plate, const std::string& model);
    bool dequeue(std::string& outPlate, std::string& outModel);
    bool isEmpty() const { return frontNode == nullptr; }
    int size() const { return count; }
    void clear();
};

// ------------------------------------------------------------------------------
// PL CO3: LIFO Maneuver History Stack
// ------------------------------------------------------------------------------
struct ManeuverNode {
    Vec2D pos;
    float angle;
    ManeuverNode* next;
    ManeuverNode(const Vec2D& p, float a) : pos(p), angle(a), next(nullptr) {}
};

class ManeuverStack {
private:
    ManeuverNode* topNode;
    int count;

public:
    ManeuverStack();
    ~ManeuverStack();
    void push(const Vec2D& pos, float angle);
    bool pop(Vec2D& outPos, float& outAngle);
    bool isEmpty() const { return topNode == nullptr; }
    int size() const { return count; }
    void clear();
};

// ------------------------------------------------------------------------------
// PL CO4 & CO5: Player Account, Salted Password Hashing & Chained Hash Table
// ------------------------------------------------------------------------------
std::string hashPassword(const std::string& pwd);

struct PlayerRecord {
    std::string name;
    std::string passwordHash;
    int totalScore;
    int levelsCompleted;
    int bestCollisions;
    float bestTime;
    PlayerRecord* next;

    PlayerRecord(const std::string& n = "", const std::string& pwdHash = "",
                 int score = 0, int lvl = 0, int col = 0, float t = 0.0f)
        : name(n), passwordHash(pwdHash), totalScore(score), levelsCompleted(lvl),
          bestCollisions(col), bestTime(t), next(nullptr) {}
};

const int HASH_BUCKETS = 16;

class PlayerHashTable {
private:
    PlayerRecord* buckets[HASH_BUCKETS];
    int totalCount;

    unsigned int hashFunction(const std::string& key) const;

public:
    PlayerHashTable();
    ~PlayerHashTable();
    void clear();
    bool insert(const PlayerRecord& rec);
    PlayerRecord* search(const std::string& name) const;
    bool remove(const std::string& name);
    int count() const { return totalCount; }
};

class PlayerDatabase {
private:
    std::vector<PlayerRecord> players;
    PlayerHashTable hashTable;
    std::string filename;

public:
    PlayerDatabase(const std::string& fname = "players.txt");
    void loadFromFile();
    void saveToFile();

    bool registerPlayer(const std::string& name, const std::string& rawPassword);
    bool authenticate(const std::string& name, const std::string& rawPassword);
    bool deletePlayer(const std::string& name);

    PlayerRecord* getPlayer(const std::string& name);
    void updatePlayerScore(const std::string& name, int scoreToAdd, int levelNumber, int collisions, float timeSec);
    void sortLeaderboard();

    const std::vector<PlayerRecord>& getPlayers() const { return players; }
};

#endif
