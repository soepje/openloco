#include <SDL3/SDL_events.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "event.h"
#include "globals.h"
#include "openloco.h"

const Uint32 myEventType = SDL_RegisterEvents(1);
static int32_t resource_id = 0;

static void PushEvent(Sint32 code, void* data1, void* data2) {
    SDL_Event event1;
    SDL_zero(event1);
    event1.type = myEventType;
    event1.user.code = code;
    event1.user.data1 = data1;
    event1.user.data2 = data2;
    SDL_PushEvent(&event1);
}

EventManager::EventManager() {
    UpdateTime();
};

void EventManager::UpdateTime() {
    std::time(&time);
}

void EventManager::TriggerEvents() {
    if (time % 10 == 0 && GAME_STATE == WORLD_VIEW) {
        // TODO
    }

    // if (time % 10 == 0 && BUILDING_MANAGER.tile_count > 0) {
    //     for (size_t i = 0; i < BUILDING_MANAGER.tile_count; i++) {
    //         auto building = BUILDING_MANAGER.tiles[i];
    //         PushEvent(0x403, building, 0);
    //         PushEvent(0x408, building, 0);
    //         SDL_Delay(25);
    //     }
    // }

    if (time % 10 == 0 && GAME_STATE == 3) {
        int32_t count = BUILDING_MANAGER.tile_count / 12;
        if (count < 2) {
            count = 2;
        }

        for (int i = count + 1; i > 0; i--) {
            auto building = BUILDING_MANAGER.tiles[rand() % BUILDING_MANAGER.tile_count];
            if (building) {
                PushEvent(0x402, building, &resource_id);
            }
        }
    }

    // TODO
}

Timestamp::Timestamp(time_t t) {
    struct tm date = *std::localtime(&t);
    month = date.tm_mon;
    day = date.tm_mday;
    hours = date.tm_hour;
    minutes = date.tm_min;
}

bool Timestamp::Between(const Timestamp &a, const Timestamp &b) {
    if (a.hours != -1 && a.minutes != -1 && b.hours != -1 && b.minutes != -1) {
        int32_t m1 = minutes + hours * 60;
        int32_t m2 = a.minutes + a.hours * 60;
        int32_t m3 = b.minutes + b.hours * 60;
        if (m2 < m3) {
            return m2 <= m1 || m1 <= m3;
        } else {
            return m2 <= m1 && m1 <= m3;
        }
    }
    return false;
}
