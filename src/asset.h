#ifndef OPENLOCO_ASSET_H
#define OPENLOCO_ASSET_H

#include <string>

#include "event.h"
#include "bitmap.h"

enum ResourceType {
    INVALID = 0,
    STARTUP = 1,
    BUILDING = 2,
    TRACK = 3,
    SCENERY = 4,
    CURSOR = 5,
    TRAIN = 6,
    MINIFIG = 7,
    VEHICLE = 8,
    TOYBOX_1 = 9,
    TOYBOX_2 = 10,
    TOYBOX_3 = 11,
    PATH = 12,
    ROAD = 13,
    MISC = 14,
};

uint32_t GetType(uint32_t id_);

struct FrameSet {
    uint16_t start_frame; // 0
    uint16_t end_frame; // 2
    uint16_t animation_delay; // 4
    int32_t restart_delay; // 8
    int16_t next_frame_set; // c
    int16_t sound; // e
    int32_t replay_delay; // 10
    uint16_t field_14; // 14
    uint8_t flip; // 16
    uint8_t split_frames; // 17
};

// size: 168
class BaseAsset {
public:
    uint32_t id; // 4
    uint32_t type; // 8

    uint32_t shadow_id; // c

    Bitmap* bitmap = nullptr; // 10
    uint16_t frame_width; // 14
    uint16_t frame_height; // 16

    int32_t field_18 = 0;

    int16_t number_of_frame_sets; // 1a

    int16_t cursor_frame_set; // 1c
    int16_t default_frame_set; // 1e

    FrameSet* frame_sets = nullptr; // 20

    Bitmap* button_bitmap = nullptr; // 24
    uint16_t button_frame_width; // 28
    uint16_t button_frame_height; // 2a

    int16_t button_frames; // 2c

    // position in toolbox grid
    int16_t button_offset_x; // 2e
    int16_t button_offset_y; // 30

    int16_t hotspot_x = 0; // 32
    int16_t hotspot_y = 0; // 34

    int32_t shadow_offset_x; // 38
    int32_t shadow_offset_y; // 3c

    int32_t must_have; // 40
    int32_t cant_have; // 44

    std::string image_name; // 48

    std::string name; // 14d

    uint32_t instances = 0; // 158
    uint32_t max_instances = 0; // 15c
    int32_t total_number_of_frames = 0; // 160

    bool success = false; // 162
    uint8_t button_visible = false; // 163
    uint32_t flags = 0; // 164

    BaseAsset(uint32_t id, const std::string& name);

    virtual ~BaseAsset() = default;
    virtual Bitmap* LoadBitmap(uint32_t width, uint32_t height);
    virtual void ReleaseBitmap();
    virtual void Load(uint32_t id, const std::string& name);
    virtual bool Parse(std::istream&);
};

class TileAsset : public BaseAsset {
public:
    uint8_t physical_occupancy_x = 0; // 168
    uint8_t physical_occupancy_y = 0; // 169
    uint8_t physical_occupancy_z = 0; // 16a

    uint8_t bitmap_occupancy_x = 0; // 16b
    uint8_t bitmap_occupancy_y = 0; // 16c

    uint8_t total_height = 0; // 16d

    uint8_t physical_occupancy[13][9][7] = {}; // 16e

    uint8_t bitmap_occupancy[13][9] = {}; // 4a1

    uint8_t max_employees = 0; // 516
    int16_t possible_employees[5] = {}; // 518

    uint8_t max_minifig = 0; // 522
    int16_t possible_minifigs[5] = {}; // 524

    int16_t rmb_seq = 0; // 52e
    int16_t closed_fs = 0; // 530

    Timestamp shift_start; // 534
    Timestamp shift_end; // 548

    Sequence insert_seq = {}; // 55c
    Sequence mobile_seq = {}; // 590
    Sequence total_visits = {}; // 5c4

    int32_t eer_replay_delay = 0; // 5f8

    Point entry_exit[4];// 5fc

    Rect free_to_roam = {}; // 61c

    uint8_t leisure_destination = 0; // 62c

    TileAsset(uint32_t id, const std::string& name);

    bool Parse(std::istream&) override;
    void ParsePhysicalOccupancy(std::istream&);
    void ParseEntryExit(std::istream&);
    static bool ParseSequence(std::istream&, Sequence&);
};

enum TrackType {
    TUNNEL_LEFT = 1,
    TUNNEL_RIGHT = 2,
    TUNNEL_TOP = 3,
    TUNNEL_BOTTOM = 4,
    BRIDGE_HORIZONTAL = 5,
    BRIDGE_VERTICAL = 6,
    DEPOT_LEFT = 7,
    DEPOT_RIGHT = 8,
    DEPOT_TOP = 9,
    DEPOT_BOTTOM = 10,
    POINTS = 11,
    SWITCH = 12,
    CROSSTRACK = 13,
    PATH_X_H = 14,
    PATH_X_V = 15,
    ROAD_X_H = 16,
    ROAD_X_V = 17,
    STATION_H = 18,
    STATION_V = 19,
};

// size: 63c
class TrackAsset : public TileAsset {
public:
    int16_t* points = nullptr; // 630
    // 634 is still a mystery
    int16_t points_x = 0; // 636
    int16_t points_y = 0; // 638
    TrackType track_type; // 63a

    TrackAsset(uint32_t id, const std::string& name);

    bool Parse(std::istream&) override;
    bool IsTunnel();
    bool IsDepot();
    bool IsStation();
};

// size: 7ac
class TrainAsset : public BaseAsset {
public:
    // TODO maybe two separate int16_t[200][2] arrays is more appropriate
    int16_t train_data[2][200][2] = {}; // 168
    int16_t speed = 0; // 7a8
    int16_t speed_reverse = 0; // 7aa

    TrainAsset(uint32_t id, const std::string& name);

    bool Parse(std::istream&) override;
    bool ParseTrainData();
};

// size: 178
class PersonAsset : public BaseAsset {
public:
    uint8_t walk_speed_a = 0; // 168
    uint8_t walk_speed_b = 0; // 169
    uint8_t ground_width = 0; // 16a
    uint8_t spawn_limit = 0; // 16b
    uint8_t employable = 0; // 16c
    char sex = 0; // 170
    uint32_t pickup_sound_id = 0; // 174

    PersonAsset(uint32_t id, const std::string& name);

    bool Parse(std::istream&) override;
};

class AssetManager  {
public:

    BaseAsset* assets[1024 * 16] = {}; // 10030

    BaseAsset* GetAsset(uint32_t id);
    bool LoadAsset(uint32_t id, const std::string& name);

};

#endif //OPENLOCO_ASSET_H
