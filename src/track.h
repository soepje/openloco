#ifndef OPENLOCO_TRACK_H
#define OPENLOCO_TRACK_H

#include "asset.h"
#include "building.h"

enum class TrackType {
    REGULAR = 0,
    SWITCH = 1,
    POINTS = 2,
    TUNNEL = 3,
    DEPOT = 4,
    BRIDGE = 5,
    CROSSTRACK = 6,
    CROSSING = 7,
    BUFFER = 8,
};

enum class TrackState {
    SWITCH_GO = 0,
    SWITCH_REVERSE = 1,
    SWITCH_STOP = 2,
    DEFAULT = 3,
    CONFIG_A = 4, // used for points and crossings
    CONFIG_B = 5, // used for points and crossings
};

class Track : public Building {
public:
    explicit Track(uint32_t resource_id);

    TrackType track_type; // 10c
    TrackState track_state; // 110
    int16_t num_trains = 0; // 114
    int32_t field_118; // 118
};

#endif //OPENLOCO_TRACK_H
