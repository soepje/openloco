#ifndef OPENLOCO_TRAIN_MANAGER_H
#define OPENLOCO_TRAIN_MANAGER_H

#include "train.h"
#include "savegame.h"
#include "building.h"
#include "depot.h"

class TrainManager {
public:
    uint16_t field_4; // 4
    uint16_t field_6; // 6
    Train* trains[4]; // 8
    uint32_t field_18[4]; // 18

    Train* AddTrain(Depot* depot, SavegameTrain* train);
    void Update();
    void DetectCollisions(Train* train);
};

#endif //OPENLOCO_TRAIN_MANAGER_H
