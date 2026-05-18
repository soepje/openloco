#ifndef OPENLOCO_BUILDING_H
#define OPENLOCO_BUILDING_H

#include "entity.h"

// size: 0x10c
class Building : public ImageEntity {
public:
    int16_t tile_x = 0; // 88
    int16_t tile_y = 0; // 8a

    uint8_t max_resident_count = 0; // 8c
    uint8_t resident_count = 0; // 8d
    uint8_t employee_count = 0; // 8e

    Entity* residents[5] = {}; // 90
    Entity* employees[5] = {}; // a4

    uint32_t field_b8 = 0; // b8
    uint32_t field_bc = 0; // bc

    // used for pathfinding minifigs
    Entity* path_neighbors[4] = {}; // c4
    int32_t field_c4[9] = {0, 0, 0, 0, -1}; // d4

    // used for pathfinding cars
    Entity* road_neighbors[4] = {}; // e8
    int32_t field_e8[9] = {0, 0, 0, 0, -1}; // f8

    explicit Building(uint32_t asset_id);

    virtual Entity* AddMinifig(int32_t resource_id);
    virtual void UnkTileEntity1();
    virtual bool UnkTileEntity2();
    virtual void UpdateClosedFrameset() { /* TODO selects the closed frameset when time outside shifts */  };

    bool GetRoamingDestination(Rect& dest);
    Point* GetEntryExit(Point* point, uint32_t direction);
};

class TrackEntity : public Building {
public:
    explicit TrackEntity(uint32_t resource_id) : Building(resource_id) {};

    // TODO
};

class TunnelEntity : public TrackEntity {
public:
    explicit TunnelEntity(uint32_t resource_id) : TrackEntity(resource_id) {};

    // TODO
};

class DepotEntity : public TrackEntity {
public:
    explicit DepotEntity(uint32_t resource_id) : TrackEntity(resource_id) {};

    // TODO
};

class BuildingManager {
public:

    std::vector<Building*> tiles; // 4
    int32_t tile_count = 0; // 14
    int32_t leisure_destinations = 0; // 18

    void Update();
    Entity *CreateBuilding(uint32_t resource_id);
    void RemoveTile(Entity* entity, bool trigger_explosion_effect);
    Building* GetRandomBuilding(int32_t type);
};

#endif //OPENLOCO_BUILDING_H
