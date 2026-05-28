#ifndef OPENLOCO_TRAIN_H
#define OPENLOCO_TRAIN_H

#include "building.h"
#include "entity.h"
#include "savegame.h"
#include <cstdint>
#include "track.h"
#include "train_bogie.h"
#include "train_car.h"

// Size: 0x94
class Train {
public:
    uint32_t field_4; // 4
    int32_t direction; // 8
    uint16_t last_car; // c
    TrainCar* cars[4]; // 10

    TrainBogie* field_20; // 20

    int16_t speed_slow; // 24
    int16_t speed_fast; // 26

    // waiting time after crash?
    int32_t field_28; // 28
    bool on_bridge; // 2c

    // some tile_x and tile_y of ???
    int16_t field_2e; // 2e
    int16_t field_30; // 30

    // tile_x and tile_y of depot initially
    int16_t field_32; // 32
    int16_t field_34; // 34

    // this is probably waiting time left at station
    int16_t field_36; // 36

    // Not sure about type
    uint8_t field_38[8]; // 38

    int16_t speed; // 58

    bool field_5a; // 5a

    // some state (0 = normal, 1 = waiting, 2 = ?, 3 = ?, 4 = crashed)
    int32_t field_5c; // 5c

    uint32_t tunnel_state; // 60
    uint32_t depot_state; // 64

    uint32_t field_68; // 68

    uint32_t field_70; // 70

    uint8_t field_78;
    uint8_t field_7a;
    uint8_t field_7c;


    bool field_88; // 88
    uint32_t field_8c; // 8c
    bool field_90; // 90



    Train(uint32_t engine_resource_id, uint32_t unk1, bool unk2, bool unk3);

    void SetVisible(bool param);
    void Unk2(int32_t param_1);
    Track* GetTrack();

    bool HasPassengerCar();
    bool IsPlainTrack();
    bool IsOnBridge();
};

#endif //OPENLOCO_TRAIN_H
