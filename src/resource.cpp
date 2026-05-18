#include "resource.h"

#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

ResourceManager::ResourceManager() {

}

ResourceManager::~ResourceManager() {
    ResourceInfo* current = res;
    while (current != nullptr) {
        ResourceInfo* next = current->next;
        delete current;
        current = next;
    }
}

bool ResourceManager::Load(const std::string& path) {
    fp = std::ifstream(path + "RFH");
    if (!fp.is_open()) {
        return false;
    }

    while (!fp.eof()) {
        uint32_t name_size = 0;
        char name[400] = {};
        uint32_t size = 0;
        uint32_t flags = 0;

        fp.read((char*)&name_size, sizeof(uint32_t));
        if (name_size != 0) {
            fp.read(name, name_size);
            fp.read((char*)&size, sizeof(uint32_t));
            fp.read((char*)&flags, sizeof(uint32_t));

            ResourceInfo* r = new ResourceInfo();
            r->name = name;
            r->flags = flags;
            r->size = size;
            r->next = nullptr;

            if (res == nullptr) {
                res = r;
            } else {
                ResourceInfo* current = res;
                while (current->next != nullptr) {
                    current = current->next;
                }
                current->next = r;
            }
        }
    }

    fp.close();

    fp = std::ifstream(path + "RFD");
    filename = path + "RFD";
    unknown = 0;

    if (!fp.is_open()) {
        return false;
    }

    return true;
}

char* ResourceManager::Get(const std::string& name, size_t& out_size) {
    if (!fp.is_open()) {
        return nullptr;
    }

    if (res != nullptr) {
        size_t offset = 0;
        ResourceInfo* current = res;
        while (current != nullptr) {
            if (current->name == name) {
                break;
            }
            offset += current->size;
            current = current->next;
        }

        if (current == nullptr) {
            return nullptr;
        }

        fp.seekg(offset);

        char* data = new char[current->size];
        fp.read(data, current->size);
        if (current->flags == 0) {
            out_size = current->size;
            return data;
        }

        char* decompressed = new char[get_decompressed_size(data)];
        decompress(data, decompressed, out_size);
        delete[] data;
        return decompressed;
    }

    return nullptr;
}

ResourceManager* ResourceManager::instance = nullptr;
ResourceManager *ResourceManager::GetInstance()  {
    if (instance == nullptr) {
        instance = new ResourceManager();
    }
    return instance;
}

size_t get_decompressed_size(const char* data) {
    return *(uint32_t*) data;
}

void decompress(const char* data, char* decompressed, size_t& decompressed_size) {
    uint32_t dec_size = get_decompressed_size(data);
    uint16_t node_0 = *(uint16_t*) (data + 4);
    uint8_t bit_index = 32;
    uint32_t byte = *(uint32_t*) (data + 0x808);
    uint32_t *next_byte = (uint32_t*) (data + 0x80c);
    for (size_t i = 0; i < dec_size; i++) {
        uint16_t node = node_0;
        while (node > 255) {
            uint8_t bit = byte & 1;
            byte >>= 1;
            size_t node_pos = (node * 4) + (bit * 2) + 8;
            bit_index--;
            node = *(uint16_t*) (data + node_pos);
            if (bit_index == 0) {
                bit_index = 32;
                byte = *next_byte;
                next_byte++;
            }
        }
        decompressed[i] = *(char*)&node;
    }
    decompressed_size = get_decompressed_size(data);
}
