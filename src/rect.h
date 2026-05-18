#ifndef LOCO_RECT_H
#define LOCO_RECT_H
#include <cstdint>

struct Rect {
    int32_t left, top, right, bottom;
};

struct Point {
    int32_t x, y;

    // TODO use where possible
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Point& rhs) const {
        return x != rhs.x || y != rhs.y;
    }
};

bool IsRectEmpty(const Rect& rect);

void SetRect(Rect& rect, int32_t left, int32_t top, int32_t right, int32_t bottom);
void SetRectEmpty(Rect& rect);
void OffsetRect(Rect& rect, int32_t x, int32_t y);

bool IntersectRect(Rect& out, const Rect& a, const Rect& b);

int32_t Distance(const Point& p1, const Point& p2);
bool PtInRect(const Rect& r, const Point& p);

#endif //LOCO_RECT_H
