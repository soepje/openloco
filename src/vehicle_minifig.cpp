#include "vehicle_minifig.h"

void VehicleMinifig::Update(Minifig *next) {
    if (field_89 == 0) {
        Minifig::Update(next);
        if (target_distance != 0) {

        }
    }
}
