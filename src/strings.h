#ifndef OPENLOCO_STRINGS_H
#define OPENLOCO_STRINGS_H

#include <cstdint>

struct StringTableEntry {
    uint32_t id;
    const char* string;
};

const char* LoadString(uint32_t id);

extern StringTableEntry StringTable[];

#endif //OPENLOCO_STRINGS_H