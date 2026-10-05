#include "depot.h"
#include "track.h"

Depot::Depot(uint32_t resource_id) : Track(resource_id) {
    field_11c = 0;
    train = 0;
    train_queue = 0;
    field_128 = 0;
    track_type = TrackType::DEPOT;
};

void Depot::QueueTrain(Train* train) {
    TrainItem* end = nullptr;
    for (TrainItem* node = train_queue; node != nullptr; node = node->next) {
        end = node;
    }
    TrainItem* new_node = new TrainItem();
    if (!end) {
        train_queue = new_node;
        new_node->train = train;
        new_node->next = nullptr;
        return;
    }
    end->next = new_node;
    new_node->train = train;
    new_node->next = nullptr;
}
