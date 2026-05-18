#ifndef OPENLOCO_WALKING_MINIFIG_H
#define OPENLOCO_WALKING_MINIFIG_H

#include "minifig.h"
#include <cstdint>

class WalkingMinifig : public Minifig {
public:

    uint32_t field_f0 = 0; // f0

    explicit WalkingMinifig(uint32_t resource_id);

    int32_t GetTimeAction(time_t time);
    bool HandleTimeAction(int32_t action);
    void SetDirectionFrameset();
    void SetMoodFrameset();
    bool WaitForTrain();

    void Update(Minifig* follow) override;
    bool IsValidTarget(Point p) override;

    virtual int32_t Move(); // 58
    virtual bool Follow(Minifig* minifig); // 5c
};

#endif //OPENLOCO_WALKING_MINIFIG_H
