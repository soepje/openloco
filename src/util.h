#ifndef OPENLOCO_UTIL_H
#define OPENLOCO_UTIL_H
#include <string>
#include <stdint.h>

// TODO rename to iequals
bool Compare(const std::string& s1, const std::string& s2);
bool Compare(const std::string& s1, const char* s2);

uint32_t uint32le(const char*);
uint16_t uint16le(const char*);

#endif //OPENLOCO_UTIL_H
