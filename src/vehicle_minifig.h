#ifndef OPENLOCO_VEHICLE_MINIFIG_H
#define OPENLOCO_VEHICLE_MINIFIG_H

#include "minifig.h"

class VehicleMinifig : public Minifig {
    // TODO
public:
    explicit VehicleMinifig(uint32_t resource_id) : Minifig(resource_id){}

    void Update(Minifig *follow) override;
};


#endif //OPENLOCO_VEHICLE_MINIFIG_H
