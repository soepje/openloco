#ifndef OPENLOCO_DEPOT_H
#define OPENLOCO_DEPOT_H

#include "track.h"

class Depot : public Track {
public:
    explicit Depot(uint32_t resource_id);

    int32_t field_11c; // field_11c
    int32_t field_120; // field_120
    int32_t field_124; // field_124
    bool field_128; // field_128
};

#endif //OPENLOCO_DEPOT_H
