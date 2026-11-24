#include "OOPS_ParkingLot.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>

static void drawBitmapText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_12) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

ParkingLot::ParkingLot(int windowWidth, int windowHeight)
    : minX(46.0f), maxX(754.0f), minY(66.0f), maxY(534.0f), currentLevel(1) {
    loadLevel(1);
}

ParkingLot::~ParkingLot() {}

void ParkingLot::loadLevel(int level) {
    currentLevel = level;
    slotManager.clear();
    staticObstacles.clear();
    dynamicObstacles.clear();
    parkedVehicles.clear();
    guidePath.clear();

    minX = 46.0f;
    maxX = 754.0f;
    minY = 66.0f;
    maxY = 534.0f;

    if (currentLevel == 1) {
        // =========================================================================
        // BLUEPRINT // LEVEL 1: BEGINNER (OCCUPIED PARKING BAYS // WIDE 130PX PATH)
        // =========================================================================

        // 1. Channeling Barriers to prevent shortcutting
        // Barrier 1: Center island blocking direct diagonal line from start to park
        staticObstacles.push_back(Obstacle(1, 390.0f, 66.0f, 35.0f, 260.0f, "Center Barrier"));
        // Barrier 2: Right divider wall isolating parking bay from center
        staticObstacles.push_back(Obstacle(2, 520.0f, 66.0f, 35.0f, 270.0f, "Right Divider Wall"));
        // Parking lot curb lines enclosing lower-left dead zone
        staticObstacles.push_back(Obstacle(3, 46.0f, 228.0f, 204.0f, 6.0f, "Lot Top Curb"));
        staticObstacles.push_back(Obstacle(4, 248.0f, 230.0f, 6.0f, 304.0f, "Lot Boundary Curb"));

        // 2. Occupied Parking Bays (Lower-Left Zone: X:46 to 250, Y:230 to 534)
        slotManager.insert(ParkingSlot("OCC_1", 116.0f, 284.0f, 60.0f, 36.0f, 0.0f, true, "MH-12-RS-1001", SLOT_COMPACT));
        slotManager.insert(ParkingSlot("OCC_2", 116.0f, 384.0f, 60.0f, 36.0f, 0.0f, true, "MH-14-BU-2022", SLOT_COMPACT));
        slotManager.insert(ParkingSlot("OCC_3", 116.0f, 484.0f, 60.0f, 36.0f, 0.0f, true, "MH-01-TX-9999", SLOT_COMPACT));

        // Parked obstacle cars in authentic blueprint colors
        parkedVehicles.push_back(ParkedCar(116.0f, 284.0f, "MH-12-RS-1001", 0.86f, 0.15f, 0.15f, 36.0f, 60.0f, 0.0f)); // Red Sedan
        parkedVehicles.push_back(ParkedCar(116.0f, 384.0f, "MH-14-BU-2022", 0.15f, 0.39f, 0.92f, 36.0f, 60.0f, 0.0f)); // Blue SUV
        parkedVehicles.push_back(ParkedCar(116.0f, 484.0f, "MH-01-TX-9999", 0.85f, 0.47f, 0.02f, 36.0f, 60.0f, 0.0f)); // Amber Sedan

        // 3. Isolated Recessed Target Parking Bay (105x95, South entry, center 660, 165)
        slotManager.insert(ParkingSlot("PARK", 660.0f, 165.0f, 105.0f, 95.0f, 90.0f, false, "", SLOT_LARGE));
        slotManager.setTarget("PARK");

        // 4. Dynamic Obstacle Turn Apex Cones (cleaned corner snag cone at 240,420)
        dynamicObstacles.insertBack(101, 380.0f, 150.0f, 20.0f, 20.0f, "Apex Cone 1");
        dynamicObstacles.insertBack(103, 510.0f, 420.0f, 20.0f, 20.0f, "Apex Cone 2");

        // 5. Continuous Drivable Navigation Guide Path (from blueprint SVG line 17)
        guidePath.push_back(Vec2D(95.0f, 160.0f));
        guidePath.push_back(Vec2D(320.0f, 160.0f));
        guidePath.push_back(Vec2D(320.0f, 430.0f));
        guidePath.push_back(Vec2D(660.0f, 430.0f));
        guidePath.push_back(Vec2D(660.0f, 230.0f));
        guidePath.push_back(Vec2D(660.0f, 165.0f));

    } else if (currentLevel == 2) {
        // =========================================================================
        // BLUEPRINT // LEVEL 2: INTERMEDIATE (PARKING SLOTS ADDED IN UPPER QUADRANT)
        // =========================================================================

        // 1. Channeling Barriers (3 Vertical Slalom Dividers anchored to walls)
        staticObstacles.push_back(Obstacle(1, 145.0f, 66.0f, 30.0f, 320.0f, "Barrier 1"));
        staticObstacles.push_back(Obstacle(2, 325.0f, 215.0f, 30.0f, 319.0f, "Barrier 2"));
        staticObstacles.push_back(Obstacle(3, 515.0f, 66.0f, 30.0f, 320.0f, "Barrier 3"));
        staticObstacles.push_back(Obstacle(4, 645.0f, 230.0f, 105.0f, 12.0f, "Wall Divider"));

        // 2. Upper East Wall Parking Bays (SLOT 1, SLOT 2) - Aligned with PARK at X=690, Width=115
        slotManager.insert(ParkingSlot("SLOT 1", 690.0f, 107.0f, 115.0f, 70.0f, 0.0f, true, "MH-12-AB-1001", SLOT_COMPACT));
        slotManager.insert(ParkingSlot("SLOT 2", 690.0f, 187.0f, 115.0f, 70.0f, 0.0f, true, "MH-04-HJ-5511", SLOT_COMPACT));

        parkedVehicles.push_back(ParkedCar(690.0f, 107.0f, "MH-12-AB-1001", 0.86f, 0.15f, 0.15f, 34.0f, 58.0f, 0.0f));
        parkedVehicles.push_back(ParkedCar(690.0f, 187.0f, "MH-04-HJ-5511", 0.15f, 0.39f, 0.92f, 34.0f, 58.0f, 0.0f));

        // 3. Main Designated Destination Bay (PARK aligned at X=690, Width=115, Height=72)
        slotManager.insert(ParkingSlot("PARK", 690.0f, 285.0f, 115.0f, 72.0f, 0.0f, false, "", SLOT_LARGE));
        slotManager.setTarget("PARK");

        // 4. Dynamic Obstacle Apex Cones (Cleaned up redundant road clutter cones)
        dynamicObstacles.insertBack(101, 150.0f, 395.0f, 20.0f, 20.0f, "Apex Cone 1");
        dynamicObstacles.insertBack(102, 330.0f, 185.0f, 20.0f, 20.0f, "Apex Cone 2");
        dynamicObstacles.insertBack(103, 520.0f, 395.0f, 20.0f, 20.0f, "Apex Cone 3");

        // 5. S-Shaped Navigation Guide Path (from blueprint SVG line 21)
        guidePath.push_back(Vec2D(90.0f, 460.0f));
        guidePath.push_back(Vec2D(235.0f, 460.0f));
        guidePath.push_back(Vec2D(235.0f, 145.0f));
        guidePath.push_back(Vec2D(435.0f, 145.0f));
        guidePath.push_back(Vec2D(435.0f, 460.0f));
        guidePath.push_back(Vec2D(675.0f, 460.0f));
        guidePath.push_back(Vec2D(675.0f, 285.0f));
        guidePath.push_back(Vec2D(690.0f, 285.0f));

    } else {
        // =========================================================================
        // BLUEPRINT // LEVEL 3: ADVANCED (OCCUPIED PARKING BAYS // ZERO SHORTCUTS)
        // =========================================================================

        // 1. Channeling Barriers (3 Long Vertical Slalom Dividers)
        staticObstacles.push_back(Obstacle(1, 190.0f, 66.0f, 25.0f, 329.0f, "Divider 1"));
        staticObstacles.push_back(Obstacle(2, 375.0f, 205.0f, 25.0f, 329.0f, "Divider 2"));
        staticObstacles.push_back(Obstacle(3, 540.0f, 66.0f, 25.0f, 329.0f, "Divider 3"));

        // Cul-De-Sac Enclosure Walls around Target Bay (repositioned for wider 105x72 slot)
        staticObstacles.push_back(Obstacle(4, 555.0f, 175.0f, 65.0f, 25.0f, "Alcove North Roof"));
        staticObstacles.push_back(Obstacle(5, 555.0f, 280.0f, 65.0f, 25.0f, "Alcove South Floor"));
        staticObstacles.push_back(Obstacle(6, 590.0f, 176.0f, 164.0f, 6.0f, "Parking Curb"));

        // 2. Occupied Parking Slots (Top-Right Zone: Aligned at X=670, Width=115)
        slotManager.insert(ParkingSlot("OCC_1", 670.0f, 95.0f, 115.0f, 52.0f, 0.0f, true, "MH-14-CD-2022", SLOT_COMPACT));
        slotManager.insert(ParkingSlot("OCC_2", 670.0f, 150.0f, 115.0f, 52.0f, 0.0f, true, "MH-02-KL-8888", SLOT_COMPACT));

        parkedVehicles.push_back(ParkedCar(670.0f, 95.0f, "MH-14-CD-2022", 0.86f, 0.15f, 0.15f, 34.0f, 58.0f, 0.0f)); // Red Sedan
        parkedVehicles.push_back(ParkedCar(670.0f, 150.0f, "MH-02-KL-8888", 0.15f, 0.39f, 0.92f, 34.0f, 58.0f, 0.0f)); // Blue SUV

        // 3. Recessed 3-Sided Dead-End Parking Bay (Wider 105x72 PARK Bay at center 612, 240, East entry)
        slotManager.insert(ParkingSlot("PARK", 612.0f, 240.0f, 105.0f, 72.0f, 180.0f, false, "", SLOT_LARGE));
        slotManager.setTarget("PARK");

        // 4. Dynamic Obstacle Apex Cones (Cleaned up redundant corner clutter cones)
        dynamicObstacles.insertBack(101, 193.0f, 406.0f, 18.0f, 18.0f, "Cone 1");
        dynamicObstacles.insertBack(102, 378.0f, 176.0f, 18.0f, 18.0f, "Cone 2");
        dynamicObstacles.insertBack(103, 543.0f, 406.0f, 18.0f, 18.0f, "Cone 3");

        // 5. Continuous Navigation Guide Path (from blueprint SVG line 21)
        guidePath.push_back(Vec2D(120.0f, 135.0f));
        guidePath.push_back(Vec2D(120.0f, 465.0f));
        guidePath.push_back(Vec2D(340.0f, 465.0f));
        guidePath.push_back(Vec2D(340.0f, 135.0f));
        guidePath.push_back(Vec2D(530.0f, 135.0f));
        guidePath.push_back(Vec2D(530.0f, 465.0f));
        guidePath.push_back(Vec2D(685.0f, 465.0f));
        guidePath.push_back(Vec2D(685.0f, 240.0f));
        guidePath.push_back(Vec2D(612.0f, 240.0f));
    }
}

// CG CO4 & CO6: Blueprint Rendering Pipeline
void ParkingLot::render(float pulseScale) {
    // 1. Blueprint Background Grid (#14171c ground + #1f2530 grid lines)
    glColor3f(0.08f, 0.09f, 0.11f);
    glRectf(0.0f, 0.0f, 800.0f, 600.0f);

    glColor3f(0.12f, 0.15f, 0.19f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (float x = 40.0f; x <= 760.0f; x += 40.0f) {
        glVertex2f(x, 60.0f); glVertex2f(x, 540.0f);
    }
    for (float y = 60.0f; y <= 540.0f; y += 40.0f) {
        glVertex2f(40.0f, y); glVertex2f(760.0f, y);
    }
    glEnd();

    // 2. Continuous Drivable Roadway Underlay (Wide 120-135px corridor #18202c)
    if (guidePath.size() >= 2) {
        glColor3f(0.09f, 0.12f, 0.17f);
        // Draw connected wide road quads along the guide path
        float halfLane = 65.0f;
        for (size_t i = 0; i < guidePath.size() - 1; i++) {
            float x1 = guidePath[i].x, y1 = guidePath[i].y;
            float x2 = guidePath[i+1].x, y2 = guidePath[i+1].y;
            float dx = x2 - x1, dy = y2 - y1;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.001f) {
                float nx = -dy / len * halfLane;
                float ny = dx / len * halfLane;
                glBegin(GL_QUADS);
                    glVertex2f(x1 - nx, y1 - ny);
                    glVertex2f(x1 + nx, y1 + ny);
                    glVertex2f(x2 + nx, y2 + ny);
                    glVertex2f(x2 - nx, y2 - ny);
                glEnd();
            }
        }
    }

    // 3. Occupied Lot Underlays & Divider Curbs
    if (currentLevel == 1) {
        // Enclosed Left Zone (X:46 to 250, Y:230 to 534)
        glColor3f(0.06f, 0.09f, 0.16f);
        glRectf(46.0f, 230.0f, 250.0f, 534.0f);

        // Closed curb line (Red #f43f5e)
        glColor3f(0.96f, 0.25f, 0.37f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(250.0f, 230.0f); glVertex2f(250.0f, 534.0f);
        glEnd();

        // Dashed stall markings
        glColor3f(0.39f, 0.45f, 0.55f);
        glLineWidth(1.5f);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0x0F0F);
        glBegin(GL_LINES);
            glVertex2f(46.0f, 334.0f); glVertex2f(250.0f, 334.0f);
            glVertex2f(46.0f, 434.0f); glVertex2f(250.0f, 434.0f);
        glEnd();
        glDisable(GL_LINE_STIPPLE);

    } else if (currentLevel == 3) {
        // Enclosed Top-Right Occupied Zone (X:590 to 754, Y:66 to 180)
        glColor3f(0.06f, 0.09f, 0.16f);
        glRectf(590.0f, 66.0f, 754.0f, 180.0f);

        // Back curb line (Red #f43f5e)
        glColor3f(0.96f, 0.25f, 0.37f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(590.0f, 66.0f); glVertex2f(590.0f, 180.0f);
        glEnd();

        glColor3f(0.39f, 0.45f, 0.55f);
        glLineWidth(1.5f);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0x0F0F);
        glBegin(GL_LINES);
            glVertex2f(590.0f, 123.0f); glVertex2f(754.0f, 123.0f);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }

    // 4. Outer Boundary Walls (Stroke width 12 #475569)
    glColor3f(0.28f, 0.33f, 0.41f);
    glLineWidth(12.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(40.0f, 60.0f);
        glVertex2f(760.0f, 60.0f);
        glVertex2f(760.0f, 540.0f);
        glVertex2f(40.0f, 540.0f);
    glEnd();

    // 5. Static Obstacles (Channeling concrete barriers with X-crossings)
    for (const auto& obs : staticObstacles) {
        obs.render();
    }

    // 6. Solid Parked Obstacle Cars
    for (size_t i = 0; i < parkedVehicles.size(); i++) {
        Vehicle* v = &parkedVehicles[i];
        v->render();
    }

    // 7. Dynamic Obstacles (Orange Apex Cones with white centers)
    DynamicObstacleNode* curr = dynamicObstacles.getHead();
    while (curr != nullptr) {
        float cx = curr->x + curr->width / 2.0f;
        float cy = curr->y + curr->height / 2.0f;
        float r = curr->width / 2.0f;

        // Outer orange cone base
        glColor3f(0.92f, 0.35f, 0.05f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int a = 0; a <= 20; a++) {
                float theta = a * (6.2831853f / 20.0f);
                glVertex2f(cx + std::cos(theta) * r, cy + std::sin(theta) * r);
            }
        glEnd();

        // White apex ring
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int a = 0; a <= 20; a++) {
                float theta = a * (6.2831853f / 20.0f);
                glVertex2f(cx + std::cos(theta) * (r * 0.45f), cy + std::sin(theta) * (r * 0.45f));
            }
        glEnd();

        curr = curr->next;
    }

    // 8. Navigation Guide Path with Chevrons
    if (guidePath.size() >= 2) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0x00FF);
        glColor4f(0.22f, 0.74f, 0.97f, 0.55f);
        glLineWidth(2.5f);
        glBegin(GL_LINE_STRIP);
        for (const auto& wp : guidePath) {
            glVertex2f(wp.x, wp.y);
        }
        glEnd();
        glDisable(GL_LINE_STIPPLE);

        // Directional Chevrons
        for (size_t i = 0; i < guidePath.size() - 1; i++) {
            float mx = (guidePath[i].x + guidePath[i+1].x) * 0.5f;
            float my = (guidePath[i].y + guidePath[i+1].y) * 0.5f;
            float dx = guidePath[i+1].x - guidePath[i].x;
            float dy = guidePath[i+1].y - guidePath[i].y;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.001f) {
                dx /= len; dy /= len;
                glColor4f(0.25f, 0.95f, 0.80f, 0.85f);
                glLineWidth(2.0f);
                glBegin(GL_LINE_STRIP);
                    glVertex2f(mx - dx * 7.0f - dy * 6.0f, my - dy * 7.0f + dx * 6.0f);
                    glVertex2f(mx + dx * 7.0f, my + dy * 7.0f);
                    glVertex2f(mx - dx * 7.0f + dy * 6.0f, my - dy * 7.0f - dx * 6.0f);
                glEnd();
            }
        }
    }

    // 9. Target Parking Slot (PARK Bay with Pulsing Beacon)
    ParkingSlot* target = slotManager.getTargetSlot();
    if (target) {
        float hw = target->width / 2.0f;
        float hh = target->height / 2.0f;

        glPushMatrix();
        glTranslatef(target->x, target->y, 0.0f);

        // Pulsing Neon Beacon
        float glow = pulseScale > 0.01f ? pulseScale : 1.0f;
        glScalef(glow, glow, 1.0f);

        glColor4f(0.98f, 0.80f, 0.08f, 0.18f); // Golden amber glow
        glRectf(-hw, -hh, hw, hh);

        // Dashed Amber Parking Bay Border
        glColor3f(0.98f, 0.80f, 0.08f);
        glLineWidth(3.0f);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(3, 0x0F0F);
        glBegin(GL_LINE_LOOP);
            glVertex2f(-hw, -hh);
            glVertex2f(hw, -hh);
            glVertex2f(hw, hh);
            glVertex2f(-hw, hh);
        glEnd();
        glDisable(GL_LINE_STIPPLE);

        // Solid tire bumper stop
        glColor3f(0.98f, 0.80f, 0.08f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(-hw + 6.0f, -hh + 5.0f);
            glVertex2f(hw - 6.0f, -hh + 5.0f);
        glEnd();

        // "PARK" Label
        glColor3f(0.98f, 0.80f, 0.08f);
        drawBitmapText(-18.0f, 5.0f, "PARK", GLUT_BITMAP_HELVETICA_18);

        glPopMatrix();
    }

    // 10. Blueprint Footer Bar
    glColor3f(0.58f, 0.64f, 0.72f);
    std::string footerText;
    if (currentLevel == 1) footerText = "BLUEPRINT // LEVEL 1: BEGINNER (OCCUPIED BAYS // 130px PATH)";
    else if (currentLevel == 2) footerText = "BLUEPRINT // LEVEL 2: INTERMEDIATE (SLALOM COURSE // EAST BAYS)";
    else footerText = "BLUEPRINT // LEVEL 3: ADVANCED (CUL-DE-SAC ALCOVE // 0 SHORTCUTS)";
    drawBitmapText(45.0f, 570.0f, footerText, GLUT_BITMAP_HELVETICA_10);

    drawBitmapText(570.0f, 570.0f, "Min Clearance: 130px | Solid Obstacles", GLUT_BITMAP_HELVETICA_10);
}

// Solid Collision Check: Wall Boundary, Static Obstacles, Dynamic Cones & Preoccupied Parked Cars
bool ParkingLot::checkCollision(const std::vector<Vec2D>& carCorners, const Vec2D& carCenter,
                                std::string& outCollisionEntity, bool& outIsOccupiedCar) {
    outIsOccupiedCar = false;

    // 1. Boundary Wall Check
    for (const auto& pt : carCorners) {
        if (pt.x < minX || pt.x > maxX || pt.y < minY || pt.y > maxY) {
            outCollisionEntity = "Wall Boundary";
            return true;
        }
    }

    // 2. Static Obstacles (Dividers, Curbs, Roofs)
    for (const auto& obs : staticObstacles) {
        if (obs.checkCollision(carCorners)) {
            outCollisionEntity = obs.getLabel();
            return true;
        }
    }

    // 3. Dynamic Linked-List Obstacles (Cones)
    DynamicObstacleNode* curr = dynamicObstacles.getHead();
    while (curr != nullptr) {
        for (const auto& pt : carCorners) {
            if (pt.x >= curr->x && pt.x <= (curr->x + curr->width) &&
                pt.y >= curr->y && pt.y <= (curr->y + curr->height)) {
                outCollisionEntity = "Dynamic Obstacle (" + curr->label + ")";
                return true;
            }
        }
        curr = curr->next;
    }

    // 4. Solid Parked Car Collision (Guarantees player cannot penetrate or drive through occupied cars)
    for (int i = 0; i < slotManager.getCount(); i++) {
        ParkingSlot& slot = slotManager.getSlot(i);
        if (slot.isOccupied) {
            float bMinX = slot.x - 30.0f, bMaxX = slot.x + 30.0f;
            float bMinY = slot.y - 18.0f, bMaxY = slot.y + 18.0f;

            // Check corner penetration
            for (const auto& pt : carCorners) {
                if (pt.x >= bMinX && pt.x <= bMaxX && pt.y >= bMinY && pt.y <= bMaxY) {
                    outCollisionEntity = "Occupied Car (" + slot.slotID + ")";
                    outIsOccupiedCar = true;
                    return true;
                }
            }

            // Check center distance overlap (AABB/OBB overlap guard)
            float dx = std::abs(carCenter.x - slot.x);
            float dy = std::abs(carCenter.y - slot.y);
            if (dx <= 46.0f && dy <= 28.0f) {
                outCollisionEntity = "Occupied Car (" + slot.slotID + ")";
                outIsOccupiedCar = true;
                return true;
            }
        }
    }

    return false;
}

void ParkingLot::switchTargetSlot() {
    int count = slotManager.getCount();
    if (count == 0) return;
    static int currentTargetIdx = 0;
    currentTargetIdx = (currentTargetIdx + 1) % count;
    while (slotManager.getSlot(currentTargetIdx).isOccupied) {
        currentTargetIdx = (currentTargetIdx + 1) % count;
    }
    slotManager.setTarget(slotManager.getSlot(currentTargetIdx).slotID);
}
