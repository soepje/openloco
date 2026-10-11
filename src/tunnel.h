#ifndef OPENLOCO_TUNNEL_H
#define OPENLOCO_TUNNEL_H

#include "track.h"
#include "train.h"

class Tunnel : public Track {
public:
    int32_t field_11c; // 11c
    int32_t field_120; // 120
    int32_t field_124; // 124

    explicit Tunnel(uint32_t resource_id) : Track(resource_id) { /* TODO */ };

    bool QueueTrain(Train* train) { /* TODO */ return false; };
};

#endif //OPENLOCO_TUNNEL_H
