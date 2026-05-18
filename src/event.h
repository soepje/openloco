#ifndef OPENLOCO_TIME_H
#define OPENLOCO_TIME_H

#include <cstdint>
#include <ctime>

struct Sequence {
    uint32_t field_0; // 0
    uint32_t count; // 4
    int32_t* seq; // 8
    int32_t new_resource_id; // c
    int16_t new_frameset; // 10
    int32_t minifig_resource_id; // 14
    int16_t minifig_frameset; // 18
    int32_t duration; // 1c
    int32_t easter_egg_resource_id; // 20
    int16_t easter_egg_frameset; // 24
    char action; // 28
    int32_t x; // 2c
    int32_t y; // 30
};

// size: 0x14
struct Timestamp {
    int32_t hours = -1;
    int32_t minutes = -1;
    int32_t day = -1;
    int32_t month = -1;

    Timestamp() = default;
    explicit Timestamp(time_t);

    bool Between(const Timestamp& start, const Timestamp& end);
};

class EventManager {
public:
    std::time_t time = 0; // 4
    int32_t field_8 = 0; // 8
    void* time_events = nullptr; // c
    int32_t counter = 0; // 10

    EventManager();
    void UpdateTime();
    void TriggerEvents();
};

#endif //OPENLOCO_TIME_H
