#include "depot.h"
#include "track.h"

Depot::Depot(uint32_t resource_id) : Track(resource_id) {
    field_11c = 0;
    field_120 = 0;
    field_124 = 0;
    field_128 = 0;
    track_type = TrackType::DEPOT;
};
