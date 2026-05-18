#ifndef OPENLOCO_MINIFIG_H
#define OPENLOCO_MINIFIG_H

#include "entity.h"
#include "rect.h"
#include <ctime>
#include <vector>

class Minifig : public ImageEntity {
public:
    uint8_t mood = 4; // 88
    uint8_t field_89 = 0; // 89

    Entity* home = nullptr; // 8c
    Entity* work = nullptr; // 90

    uint32_t some_timestamp = 0; // 94
    Point field_98 = { -1, -1 }; // 98

    uint32_t some_timestamp3 = 0; // a0
    uint32_t some_timestamp2 = 0; // a4

    Point field_a8 = { -1, -1 }; // a8
    Point field_b0 = { -1, -1 }; // b0
    Point field_c4 = { -1, -1 }; // c4
    Point field_cc = { -1, -1 }; // cc

    int32_t target_distance_x = -1; // d4
    int32_t target_distance_y = -1; // d8
    uint32_t target_distance = 0; // dc

    uint32_t field_e0 = 0; // e0
    uint8_t field_e4 = 0; // e4
    uint32_t current_action = 0; // e8

    explicit Minifig(uint32_t resource_id);

    void DrawFrame(const Rect &rect, bool unk, uint32_t flags) override;
    void SetName(const std::string& name) override;

    virtual void Update(Minifig* follow); // 3c
    virtual void SetDestination(Point p); // 40
    virtual void SetDestination(Entity* destination); // 44
    virtual void RoamingUnk4(Point p); // 48
    virtual void RoamingUnk5(); // 4c
    virtual void SetWork(); // 50
    virtual bool IsValidTarget(Point x); // 54

    Point& SetTargetDistance(Point& point, Point p2, int32_t limit);
};

class MinifigManager {
public:
    uint32_t minifig_count; // 3c
    uint32_t vehicle_count; // 40
    bool party = false; // 44
    time_t party_started = 0; // 48
    std::vector<Minifig*> minifigs; // 4c
    std::vector<Minifig*> vehicles; // 64

    void Draw(int32_t z, Rect r, bool unk);
    void Update();
    Minifig* CreateMinifig(uint32_t resource_id, Entity* home, int32_t x, int32_t y);
};


#endif //OPENLOCO_MINIFIG_H
