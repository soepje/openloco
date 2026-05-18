#ifndef OPENLOCO_WORLD_H
#define OPENLOCO_WORLD_H
#include "entity.h"
#include "building.h"
#include <bitset>
#include <cstdint>
#include <vector>

// Size: 0x40
struct Tile {
    Building* physical_occupancy[7] = {}; // 0
    Building* bitmap_occupancy[7] = {}; // 1c
    int8_t max_bitmap_z = -1; // 38
    int8_t max_physical_z = -1; // 39
};

struct TileIndex {
    int16_t x = -1;
    int16_t y = -1;

    TileIndex() = default;
    explicit TileIndex(Point p) {
        x = p.x < 0 ? -1 : p.x / 16;
        y = p.y < 0 ? -1 : p.y / 16;
    }
};

// Size: 0x2c
struct PathThingy {
    uint32_t field_0; // 0
    uint32_t field_4; // 4
    void* field_8; // 8
    uint16_t field_c; // c
    void* field_10; // 10

    // TODO hole

    void* field_20; // 20
    void* field_24; // 24
    void* field_28; // 28

    explicit PathThingy(uint16_t arg);
};

class World {
public:
    int32_t screen_width; // 4
    int32_t screen_height; // 8

    int32_t screen_width2; // 14
    int32_t screen_height2; // 18

    int32_t offset_x; // 1c
    int32_t offset_y; // 20

    int32_t center_x; // 24
    int32_t center_y; // 28

    int32_t center_x2; // 2c
    int32_t center_y2; // 30

    int16_t tiles_x; // 3e
    int16_t tiles_y; // 40

    Tile grid[81][65] = {}; // 48

    std::bitset<81*65> dirty; // 52484

    PathThingy* field_52488; // 52488
    PathThingy* field_5248c; // 5248c

    bool field_52490 = false;

    void UpdateDimensions();
    void Draw();
    void DrawRect(const Rect& r);
    Entity* AddTile(uint32_t resource_id, int16_t x, int16_t y, bool no_offset, uint32_t unk2);
    Entity* GetTile(int16_t x, int16_t y, int16_t z);
    Entity* GetNeighborTile(Entity* entity, uint32_t direction);
};

#endif //OPENLOCO_WORLD_H
