#ifndef OPENLOCO_TUNNEL_H
#define OPENLOCO_TUNNEL_H

#include "track.h"

class Tunnel : public Track {
public:
    explicit Tunnel(uint32_t resource_id) : Track(resource_id) { /* TODO */ };

    int32_t field_11c; // 11c
    int32_t field_120; // 120
    int32_t field_124; // 124
};

#endif //OPENLOCO_TUNNEL_H
