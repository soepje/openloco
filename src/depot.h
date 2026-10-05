#ifndef OPENLOCO_DEPOT_H
#define OPENLOCO_DEPOT_H

#include "track.h"

class Train;

struct TrainItem {
    Train* train; // 0
    TrainItem* next; // 4
};

class Depot : public Track {
public:
    explicit Depot(uint32_t resource_id);

    int32_t field_11c; // field_11c
    Train* train; // field_120
    TrainItem* train_queue; // field_124
    bool field_128; // field_128

    void QueueTrain(Train* train);
};

#endif //OPENLOCO_DEPOT_H
