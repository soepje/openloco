#include "util.h"

#include <string>

bool Compare(const std::string& s1, const std::string& s2) {
    if (s1.length() != s2.length())
        return false;

    for (int i = 0; i < s1.length(); ++i) {
        if (std::tolower(s1[i]) != std::tolower(s2[i]))
            return false;
    }

    return true;
}

bool Compare(const std::string &s1, const char *s2) {
    std::string s3(s2);
    return Compare(s1, s3);
}

uint16_t uint16le(const char *buf) {
    return (buf[0] << 0) | ((unsigned) buf[1] << 8);
}

uint32_t uint32le(const char *buf) {
    return (buf[0] << 0) | (buf[1] << 8) | (buf[2] << 16) | ((unsigned) buf[3] << 24);
}
