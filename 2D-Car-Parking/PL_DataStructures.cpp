#include "PL_DataStructures.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdio>

// ------------------------------------------------------------------------------
// OOP CO3: Number Plate Validator
// Format: MH12AB1234 or MH-12-AB-1234
// ------------------------------------------------------------------------------
bool validPlate(const std::string& n) {
    if (n.size() == 10) {
        for (int i = 0; i < 10; i++) {
            unsigned char c = (unsigned char)n[i];
            bool letterPos = (i < 2 || (i >= 4 && i < 6));
            if (letterPos ? !std::isupper(c) : !std::isdigit(c)) return false;
        }
        return true;
    } else if (n.size() == 14) {
        return (std::isupper(n[0]) && std::isupper(n[1]) && n[2] == '-' &&
                std::isdigit(n[3]) && std::isdigit(n[4]) && n[5] == '-' &&
                std::isupper(n[6]) && std::isupper(n[7]) && n[8] == '-' &&
                std::isdigit(n[9]) && std::isdigit(n[10]) && std::isdigit(n[11]) && std::isdigit(n[12]));
    }
    return false;
}

// ------------------------------------------------------------------------------
// ParkingSlot Implementation
// ------------------------------------------------------------------------------
ParkingSlot::ParkingSlot()
    : slotID(""), x(0), y(0), width(95.0f), height(50.0f),
      targetAngle(0.0f), isOccupied(false), isTarget(false), vehicleReg(""), slotType(SLOT_COMPACT) {}

ParkingSlot::ParkingSlot(std::string id, float _x, float _y, float w, float h,
                         float angle, bool occ, std::string reg, SlotType type)
    : slotID(id), x(_x), y(_y), width(w), height(h),
      targetAngle(angle), isOccupied(occ), isTarget(false), vehicleReg(reg), slotType(type) {}

bool ParkingSlot::isParked(float carX, float carY, float carAngleDeg, float tolPos, float tolAngle) const {
    float dx = std::abs(carX - x);
    float dy = std::abs(carY - y);
    if (dx > tolPos || dy > tolPos) return false;
    float diff = std::fmod(std::abs(carAngleDeg - targetAngle), 360.0f);
    if (diff > 180.0f) diff = 360.0f - diff;
    return diff <= tolAngle;
}

// ------------------------------------------------------------------------------
// PL CO1: Contiguous Fixed Array Manager
// ------------------------------------------------------------------------------
SlotArrayManager::SlotArrayManager() : count(0) {}

void SlotArrayManager::clear() {
    count = 0;
}

bool SlotArrayManager::insert(const ParkingSlot& slot) {
    if (count >= MAX_SLOTS) return false;
    slots[count++] = slot;
    return true;
}

bool SlotArrayManager::deleteByIndex(int index) {
    if (index < 0 || index >= count) return false;
    for (int i = index; i < count - 1; i++) {
        slots[i] = slots[i + 1];
    }
    count--;
    return true;
}

int SlotArrayManager::linearSearch(const std::string& id) const {
    for (int i = 0; i < count; i++) {
        if (slots[i].slotID == id) return i;
    }
    return -1;
}

int SlotArrayManager::binarySearch(const std::string& id) const {
    int low = 0, high = count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (slots[mid].slotID == id) return mid;
        if (slots[mid].slotID < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

bool SlotArrayManager::updateOccupancy(int index, bool occ, const std::string& reg) {
    if (index < 0 || index >= count) return false;
    slots[index].isOccupied = occ;
    slots[index].vehicleReg = reg;
    return true;
}

void SlotArrayManager::display() const {
    std::cout << "=== Slot Array [" << count << " slots] ===\n";
    for (int i = 0; i < count; i++) {
        std::cout << "[" << i << "] " << slots[i].slotID
                  << " | x=" << slots[i].x << " y=" << slots[i].y
                  << " | Occ=" << slots[i].isOccupied
                  << " | Target=" << slots[i].isTarget
                  << " | Type=" << (slots[i].slotType == SLOT_LARGE ? "LARGE" : "COMPACT") << "\n";
    }
}

void SlotArrayManager::setTarget(const std::string& id) {
    for (int i = 0; i < count; i++) {
        slots[i].isTarget = (slots[i].slotID == id);
    }
}

ParkingSlot* SlotArrayManager::getTargetSlot() {
    for (int i = 0; i < count; i++) {
        if (slots[i].isTarget) return &slots[i];
    }
    return nullptr;
}

// ------------------------------------------------------------------------------
// OOP CO4: Smart Slot Decision & Fee Calculation
// ------------------------------------------------------------------------------
int chooseBestSlot(bool isLargeCar, int wantedSlotNum, SlotArrayManager& slotMgr) {
    // Rule 1: Requested slot if free and fits
    for (int i = 0; i < slotMgr.getCount(); i++) {
        ParkingSlot& s = slotMgr.getSlot(i);
        int sNum = i + 1;
        bool fits = !isLargeCar || (s.slotType == SLOT_LARGE);
        if (sNum == wantedSlotNum && !s.isOccupied && fits) {
            return i;
        }
    }

    // Rule 2: Scoring: distance + waste (lowest wins)
    int bestIdx = -1;
    int bestScore = 100000;
    for (int i = 0; i < slotMgr.getCount(); i++) {
        ParkingSlot& s = slotMgr.getSlot(i);
        if (s.isOccupied) continue;
        bool fits = !isLargeCar || (s.slotType == SLOT_LARGE);
        if (!fits) continue;

        int sNum = i + 1;
        int distance = std::abs(sNum - wantedSlotNum);
        int waste = (!isLargeCar && s.slotType == SLOT_LARGE) ? 10 : 0;
        int total = distance + waste;

        if (total < bestScore) {
            bestScore = total;
            bestIdx = i;
        }
    }

    // Rule 3: Nothing fits -> throw exception
    if (bestIdx == -1) {
        throw LotFullException("No suitable parking slot available");
    }
    return bestIdx;
}

int calculateParkingFee(bool isLargeCar, int hours) {
    if (hours < 1) hours = 1;
    int fee = 0;
    if (hours <= 1) fee = 20;
    else if (hours <= 4) fee = 20 + 15 * (hours - 1);
    else fee = std::min(100, 65 + 10 * (hours - 4));

    if (isLargeCar) fee = fee * 3 / 2;
    return fee;
}

// ------------------------------------------------------------------------------
// PL CO2: Singly Linked List Dynamic Obstacles
// ------------------------------------------------------------------------------
DynamicObstacleList::DynamicObstacleList() : head(nullptr), nodeCount(0) {}

DynamicObstacleList::~DynamicObstacleList() {
    clear();
}

void DynamicObstacleList::insertFront(int id, float x, float y, float w, float h, const std::string& label) {
    DynamicObstacleNode* node = new DynamicObstacleNode(id, x, y, w, h, label);
    node->next = head;
    head = node;
    nodeCount++;
}

void DynamicObstacleList::insertBack(int id, float x, float y, float w, float h, const std::string& label) {
    DynamicObstacleNode* node = new DynamicObstacleNode(id, x, y, w, h, label);
    if (!head) {
        head = node;
    } else {
        DynamicObstacleNode* curr = head;
        while (curr->next) curr = curr->next;
        curr->next = node;
    }
    nodeCount++;
}

bool DynamicObstacleList::deleteByID(int id) {
    if (!head) return false;
    if (head->id == id) {
        DynamicObstacleNode* tmp = head;
        head = head->next;
        delete tmp;
        nodeCount--;
        return true;
    }
    DynamicObstacleNode* curr = head;
    while (curr->next && curr->next->id != id) curr = curr->next;
    if (curr->next) {
        DynamicObstacleNode* tmp = curr->next;
        curr->next = tmp->next;
        delete tmp;
        nodeCount--;
        return true;
    }
    return false;
}

void DynamicObstacleList::clear() {
    while (head) {
        DynamicObstacleNode* tmp = head;
        head = head->next;
        delete tmp;
    }
    nodeCount = 0;
}

void DynamicObstacleList::displayList() const {
    std::cout << "=== Dynamic Obstacle List [" << nodeCount << " nodes] ===\n";
    DynamicObstacleNode* curr = head;
    int idx = 0;
    while (curr) {
        std::cout << "[" << idx++ << "] ID=" << curr->id
                  << " Label=" << curr->label
                  << " x=" << curr->x << " y=" << curr->y << "\n";
        curr = curr->next;
    }
}

// ------------------------------------------------------------------------------
// PL CO3: FIFO Waiting Entry Queue
// ------------------------------------------------------------------------------
CarQueue::CarQueue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

CarQueue::~CarQueue() {
    clear();
}

void CarQueue::enqueue(const std::string& plate, const std::string& model) {
    QueuedCar* node = new QueuedCar(plate, model);
    if (!rearNode) {
        frontNode = rearNode = node;
    } else {
        rearNode->next = node;
        rearNode = node;
    }
    count++;
}

bool CarQueue::dequeue(std::string& outPlate, std::string& outModel) {
    if (!frontNode) return false;
    QueuedCar* tmp = frontNode;
    outPlate = tmp->plate;
    outModel = tmp->model;
    frontNode = frontNode->next;
    if (!frontNode) rearNode = nullptr;
    delete tmp;
    count--;
    return true;
}

void CarQueue::clear() {
    while (frontNode) {
        QueuedCar* tmp = frontNode;
        frontNode = frontNode->next;
        delete tmp;
    }
    rearNode = nullptr;
    count = 0;
}

// ------------------------------------------------------------------------------
// PL CO3: LIFO Maneuver History Stack
// ------------------------------------------------------------------------------
ManeuverStack::ManeuverStack() : topNode(nullptr), count(0) {}

ManeuverStack::~ManeuverStack() {
    clear();
}

void ManeuverStack::push(const Vec2D& pos, float angle) {
    ManeuverNode* node = new ManeuverNode(pos, angle);
    node->next = topNode;
    topNode = node;
    count++;
}

bool ManeuverStack::pop(Vec2D& outPos, float& outAngle) {
    if (!topNode) return false;
    ManeuverNode* tmp = topNode;
    outPos = tmp->pos;
    outAngle = tmp->angle;
    topNode = topNode->next;
    delete tmp;
    count--;
    return true;
}

void ManeuverStack::clear() {
    while (topNode) {
        ManeuverNode* tmp = topNode;
        topNode = topNode->next;
        delete tmp;
    }
    count = 0;
}

// ------------------------------------------------------------------------------
// PL CO4 & CO5: Salted Password Hashing & Player Hash Table
// ------------------------------------------------------------------------------
std::string hashPassword(const std::string& pwd) {
    unsigned long long h = 14695981039346656037ULL;
    const std::string salt = "CarPark2026_CO_Sec#";
    std::string combined = salt + pwd;
    for (char c : combined) {
        h ^= (unsigned char)c;
        h *= 1099511628211ULL;
    }
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%016llx", h);
    return std::string(buf);
}

PlayerHashTable::PlayerHashTable() : totalCount(0) {
    for (int i = 0; i < HASH_BUCKETS; i++) buckets[i] = nullptr;
}

PlayerHashTable::~PlayerHashTable() {
    clear();
}

void PlayerHashTable::clear() {
    for (int i = 0; i < HASH_BUCKETS; i++) {
        PlayerRecord* curr = buckets[i];
        while (curr) {
            PlayerRecord* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
        buckets[i] = nullptr;
    }
    totalCount = 0;
}

unsigned int PlayerHashTable::hashFunction(const std::string& key) const {
    unsigned int h = 5381;
    for (char c : key) {
        h = ((h << 5) + h) + (unsigned char)c;
    }
    return h % HASH_BUCKETS;
}

bool PlayerHashTable::insert(const PlayerRecord& rec) {
    if (search(rec.name)) return false;
    unsigned int idx = hashFunction(rec.name);
    PlayerRecord* node = new PlayerRecord(rec);
    node->next = buckets[idx];
    buckets[idx] = node;
    totalCount++;
    return true;
}

PlayerRecord* PlayerHashTable::search(const std::string& name) const {
    unsigned int idx = hashFunction(name);
    PlayerRecord* curr = buckets[idx];
    while (curr) {
        if (curr->name == name) return curr;
        curr = curr->next;
    }
    return nullptr;
}

bool PlayerHashTable::remove(const std::string& name) {
    unsigned int idx = hashFunction(name);
    PlayerRecord* curr = buckets[idx];
    PlayerRecord* prev = nullptr;
    while (curr) {
        if (curr->name == name) {
            if (prev) prev->next = curr->next;
            else buckets[idx] = curr->next;
            delete curr;
            totalCount--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

// ------------------------------------------------------------------------------
// PlayerDatabase Implementation
// ------------------------------------------------------------------------------
PlayerDatabase::PlayerDatabase(const std::string& fname) : filename(fname) {}

void PlayerDatabase::loadFromFile() {
    players.clear();
    hashTable.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string name, pwdHash, sScore, sLvl, sCol, sTime;

        std::getline(ss, name, ',');
        std::getline(ss, pwdHash, ',');
        std::getline(ss, sScore, ',');
        std::getline(ss, sLvl, ',');
        std::getline(ss, sCol, ',');
        std::getline(ss, sTime, ',');

        PlayerRecord rec;
        rec.name = name;
        if (!sTime.empty()) {
            // New 6-column format: name,pwdHash,score,lvl,col,time
            rec.passwordHash = pwdHash;
            rec.totalScore = std::stoi(sScore);
            rec.levelsCompleted = std::stoi(sLvl);
            rec.bestCollisions = std::stoi(sCol);
            rec.bestTime = std::stof(sTime);
        } else if (!sCol.empty()) {
            // Legacy 5-column format: name,score,lvl,col,time (default pwd: "1234")
            rec.passwordHash = hashPassword("1234");
            rec.totalScore = std::stoi(pwdHash);
            rec.levelsCompleted = std::stoi(sScore);
            rec.bestCollisions = std::stoi(sLvl);
            rec.bestTime = std::stof(sCol);
        }

        if (!rec.name.empty()) {
            players.push_back(rec);
            hashTable.insert(rec);
        }
    }
}

void PlayerDatabase::saveToFile() {
    std::ofstream file(filename);
    for (const auto& p : players) {
        file << p.name << "," << p.passwordHash << "," << p.totalScore << ","
             << p.levelsCompleted << "," << p.bestCollisions << "," << p.bestTime << "\n";
    }
}

bool PlayerDatabase::registerPlayer(const std::string& name, const std::string& rawPassword) {
    if (name.empty() || rawPassword.empty()) return false;
    if (hashTable.search(name) != nullptr) return false;

    std::string hPwd = hashPassword(rawPassword);
    PlayerRecord rec(name, hPwd, 0, 0, 0, 0.0f);
    players.push_back(rec);
    hashTable.insert(rec);
    saveToFile();
    return true;
}

bool PlayerDatabase::authenticate(const std::string& name, const std::string& rawPassword) {
    PlayerRecord* rec = hashTable.search(name);
    if (!rec) return false;
    return rec->passwordHash == hashPassword(rawPassword);
}

bool PlayerDatabase::deletePlayer(const std::string& name) {
    hashTable.remove(name);
    bool found = false;
    for (auto it = players.begin(); it != players.end(); ++it) {
        if (it->name == name) {
            players.erase(it);
            found = true;
            break;
        }
    }
    if (found) saveToFile();
    return found;
}

PlayerRecord* PlayerDatabase::getPlayer(const std::string& name) {
    return hashTable.search(name);
}

void PlayerDatabase::updatePlayerScore(const std::string& name, int scoreToAdd,
                                       int levelNumber, int collisions, float timeSec) {
    PlayerRecord* p = getPlayer(name);
    if (!p) {
        registerPlayer(name, "1234");
        p = getPlayer(name);
        if (!p) return;
    }
    p->totalScore += scoreToAdd;
    if (levelNumber > p->levelsCompleted) p->levelsCompleted = levelNumber;
    if (p->bestCollisions == 0 || collisions < p->bestCollisions) p->bestCollisions = collisions;
    if (p->bestTime == 0.0f || timeSec < p->bestTime) p->bestTime = timeSec;

    for (auto& item : players) {
        if (item.name == name) {
            item.totalScore = p->totalScore;
            item.levelsCompleted = p->levelsCompleted;
            item.bestCollisions = p->bestCollisions;
            item.bestTime = p->bestTime;
            break;
        }
    }
    saveToFile();
}

void PlayerDatabase::sortLeaderboard() {
    int n = (int)players.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (players[j].totalScore < players[j + 1].totalScore) {
                PlayerRecord tmp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = tmp;
            }
        }
    }
}
