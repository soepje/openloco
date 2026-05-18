#ifndef OPENLOCO_TRAIN_H
#define OPENLOCO_TRAIN_H

#include "building.h"
#include "entity.h"
#include "savegame.h"
#include <cstdint>



// Size: 0x450
class TrainSegment : ImageEntity {
public:
    uint8_t field_88[0x39c]; // 88
    bool field_424; // 424
    uint32_t field_428; // 428
    uint32_t field_42c; // 42c
    void* field_430; // 430
    void* field_434; // 434
    uint16_t field_438; // 438
    uint16_t field_43a; // 43a
    bool field_43c; // 43c
    uint32_t field_440; // 440
    uint32_t field_444; // 444
    uint16_t field_448; // 448
    uint32_t field_44c; // 44c

    TrainSegment(uint32_t resource_id, uint32_t unk1, bool unk2);

    virtual bool SetAsset(uint32_t asset_id, int32_t frame_set);
};

// Size: 0x94
class Train {
    uint32_t field_4; // 4


    uint16_t last_segment; // c
    TrainSegment* segments[4]; // 10

    void* field_20; // 20

    uint32_t field_28; // 28
    bool field_2c; // 2c

    int16_t field_2e; // 2e
    int16_t field_30; // 30
    int16_t field_32; // 32
    int16_t field_34; // 34

    // Not sure about type
    uint8_t field_38[8]; // 38

    bool field_5a; // 5a

    uint32_t field_60; // 60


    uint32_t field_68; // 68

    uint32_t field_70; // 70

    bool field_88; // 88
    uint32_t field_8c; // 8c
    bool field_90; // 90



    Train(uint32_t engine_resource_id, uint32_t unk1, bool unk2, bool unk3);
};

class TrainManager {
    uint16_t field_4; // 4
    uint16_t field_6; // 6
    Train* field_8[4]; // 8
    uint32_t field_18[4]; // 18

    Train* AddTrain(DepotEntity* depot, SavegameTrain* train);
};

#endif //OPENLOCO_TRAIN_H
