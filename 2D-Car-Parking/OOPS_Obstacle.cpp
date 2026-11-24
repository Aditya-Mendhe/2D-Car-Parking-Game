#include "OOPS_Obstacle.h"
#include <GL/gl.h>
#include <GL/glut.h>

Obstacle::Obstacle()
    : id(0), x(0.0f), y(0.0f), width(10.0f), height(10.0f), label("") {}

Obstacle::Obstacle(int _id, float _x, float _y, float _w, float _h, const std::string& _lbl)
    : id(_id), x(_x), y(_y), width(_w), height(_h), label(_lbl) {}

Obstacle::Obstacle(const Obstacle& other)
    : id(other.id), x(other.x), y(other.y), width(other.width), height(other.height), label(other.label) {}

Obstacle::~Obstacle() {}

void Obstacle::render() const {
    // Fill with blueprint dark slate #1e293b
    glColor3f(0.12f, 0.16f, 0.23f);
    glRectf(x, y, x + width, y + height);

    // Diagonal structural crossing lines (blueprint pattern)
    if (width > 8.0f && height > 8.0f) {
        glColor3f(0.20f, 0.25f, 0.33f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
            glVertex2f(x, y); glVertex2f(x + width, y + height);
            glVertex2f(x + width, y); glVertex2f(x, y + height);
        glEnd();
    }

    // Border with blueprint stroke #64748b
    glColor3f(0.39f, 0.45f, 0.55f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
}

bool Obstacle::checkCollision(const Vec2D& pt) const {
    return (pt.x >= x && pt.x <= (x + width) &&
            pt.y >= y && pt.y <= (y + height));
}

bool Obstacle::checkCollision(const std::vector<Vec2D>& corners) const {
    for (const auto& pt : corners) {
        if (checkCollision(pt)) {
            return true;
        }
    }
    return false;
}
