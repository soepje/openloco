#ifndef OPENLOCO_TRAIN_SEGMENT_H
#define OPENLOCO_TRAIN_SEGMENT_H

#include "building.h"
#include "entity.h"
#include "savegame.h"
#include <cstdint>

class TrainBogie;
class Train;

enum class TrainCarType {
    INVALID = 0,
    ENGINE = 1,
    PASSENGER = 2,
    FREIGHT = 3,
    MAIL = 4,
};

// Size: 0x450
class TrainCar : public ImageEntity {
public:
    uint8_t field_88[0x39c]; // 88
    bool field_424; // 424
    uint32_t resource_id; // 428
    TrainCarType train_car_type; // 42c
    TrainBogie* bogie_front; // 430
    TrainBogie* bogie_back; // 434
    uint16_t rotation; // 438
    uint16_t field_43a; // 43a
    bool field_43c; // 43c
    uint32_t tunnel_state; // 440
    uint32_t depot_state; // 444
    uint16_t on_bridge; // 448
    Train* train; // 44c

    TrainCar(uint32_t resource_id, TrainCarType type, bool tunnel);

    virtual bool SetAsset(uint32_t asset_id, int32_t frame_set);

    bool UpdateDepotEnterVisibility(Train* train);
    bool UpdateDepotExitVisibility(Train* train);
    bool UpdateTunnelEnter(Train *train);
    bool UpdateTunnelExit(Train *train);
    void UpdateDepot(Train* train);
    void Update(Train* train);
    void UpdatePosition();
    void UpdateRotation();
    bool MoveBridge(Train* train);

    static TrainCarType GetTrainCarType(int resource_id);
};

#endif //OPENLOCO_TRAIN_SEGMENT_H
