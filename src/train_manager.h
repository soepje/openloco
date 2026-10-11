#ifndef OPENLOCO_TRAIN_MANAGER_H
#define OPENLOCO_TRAIN_MANAGER_H

#include "train.h"
#include "savegame.h"
#include "building.h"
#include "depot.h"

class TrainManager {
public:
    uint16_t num_trains; // 4
    uint16_t num_trains2; // 6
    Train* trains[4]; // 8
    TrainCar* train_cars[16]; // 18

    TrainManager();

    Train* AddTrain(Depot* depot, SavegameTrain* train);
    void Update();
    void DetectCollisions(Train* train);
    void Draw(Rect r, int16_t z);
    void UpdateTrainCars();
    void ClearTrainCars();
};

#endif //OPENLOCO_TRAIN_MANAGER_H
