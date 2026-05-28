#ifndef OPENLOCO_TRAIN_BOGIE_H
#define OPENLOCO_TRAIN_BOGIE_H

#include "building.h"
#include "entity.h"
#include "savegame.h"
#include "track.h"
#include <cstdint>

class Train;
class TrainCar;

// Size: 0x20
class TrainBogie {
public:
    int32_t direction; // 4
    int32_t point; // 8
    int32_t x; // c
    int32_t y; // 10
    Track* track; // 14
    int32_t tunnel_state; // 18
    int32_t depot_state; // 1c

    TrainBogie(bool tunnel);

    void CheckDepot();
    void CheckTunnel();
    bool CheckStation(Train* train);
    bool SetTrack(int32_t x, int32_t y);
    Track* UpdateTrack();
    bool UpdatePoint(int32_t x, int32_t y);
    bool ExitDepot();
    void Despawn();
    bool UpdateTrackConfig(Train* train, Track* track);
    bool Unk2(Train* train);
    void ReverseDirection(Train* train);
    bool Move(Train* train, TrainCar* train_segment);
};

#endif //OPENLOCO_TRAIN_BOGIE_H
