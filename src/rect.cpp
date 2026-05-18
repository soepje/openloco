#include "rect.h"

#include <algorithm>

// https://github.com/wine-mirror/wine/blob/master/dlls/user32/uitools.c

void SetRect(Rect& rect, int32_t left, int32_t top, int32_t right, int32_t bottom) {
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;
}

void SetRectEmpty(Rect& rect) {
    rect.left = rect.top = rect.right = rect.bottom = 0;
}

void OffsetRect(Rect& rect, int32_t x, int32_t y) {
    rect.left += x;
    rect.right += x;
    rect.top += y;
    rect.bottom += y;
}

bool IsRectEmpty(const Rect &rect) {
    return rect.left >= rect.right || rect.top >= rect.bottom;
}

bool IntersectRect(Rect &out, const Rect &a, const Rect &b) {
    if (IsRectEmpty(a) || IsRectEmpty(b) ||
        a.left >= b.right || b.left >= a.right ||
        a.top >= b.bottom || b.top >= a.bottom) {
        SetRectEmpty(out);
        return false;
    }
    out.left = std::max(a.left, b.left);
    out.right = std::min(a.right, b.right);
    out.top = std::max(a.top, b.top);
    out.bottom = std::min(a.bottom, b.bottom);
    return true;
}

int32_t Distance(const Point &p1, const Point &p2) {
    return (p1.y - p2.y)*(p1.y - p2.y) + (p1.x - p2.x)*(p1.x - p2.x);
}

bool PtInRect(const Rect &r, const Point &p) {
    return p.x >= r.left && p.x <= r.right && p.y >= r.top && p.y <= r.bottom; // TOOD check <= right and <= bottom
}
