#ifndef LOCO_RESOURCE_H
#define LOCO_RESOURCE_H
#include <cstdint>
#include <fstream>
#include <list>
#include <string>

struct ResourceInfo {
    std::string name; // 0
    uint32_t flags; // 4
    uint32_t size; // 8
    ResourceInfo* next; // c
};

size_t get_decompressed_size(const char* data);
void decompress(const char* data, char *output, size_t& size);


class ResourceManager {
    std::ifstream fp; // 0
    ResourceInfo* res = nullptr; // 4
    std::string filename; // 8
    uint32_t unknown = 0; // c

protected:
    ResourceManager();

    static ResourceManager* instance;

public:
    ~ResourceManager();
    ResourceManager(ResourceManager &other) = delete;
    void operator=(const ResourceManager &) = delete;
    static ResourceManager* GetInstance();

    bool Load(const std::string& filename);
    char* Get(const std::string& name, size_t& size);
};

#endif //LOCO_RESOURCE_H