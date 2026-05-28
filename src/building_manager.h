#ifndef OPENLOCO_BUILDING_MANAGER_H
#define OPENLOCO_BUILDING_MANAGER_H

#include "building.h"

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

#endif //OPENLOCO_BUILDING_MANAGER_H
