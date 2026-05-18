#include <cstdint>
#include <fstream>
#include <istream>
#include <stdexcept>
#include <string>

#include "globals.h"
#include "util.h"
#include "savegame.h"

void Savegame::Open(const std::string& path) {
    Close();

    // TODO possibly read from asset manager
    is = new std::ifstream(path);

    char header_buf[0x114];
    is->read(header_buf, 0x114);
    if (is->gcount() != 0x114) {
        throw std::runtime_error("invalid savegame header");
    }

    header.version = uint16le(header_buf);
    header.tiles_x = uint16le(header_buf + 2);
    header.tiles_y = uint16le(header_buf + 4);
    header.building_count = uint32le(header_buf + 8);
    header.train_count = uint16le(header_buf + 12);
    header.backdrop = std::string(header_buf + 14);

    uint32_t thumbnail_size = header.tiles_x * header.tiles_y;
    thumbnail = new uint8_t[thumbnail_size];
    is->read((char*)thumbnail, thumbnail_size);
    if (is->gcount() != thumbnail_size) {
        throw new std::runtime_error("invalid savegame thumbnail");
    }
}

void Savegame::Close() {
    // TODO
}

bool Savegame::CheckVersion() {
    return header.version == 8;
}

SavegameBuilding* Savegame::ReadBuilding() {
    char buf[0x80];
    is->read(buf, 0x80);
    if (is->gcount() != 0x80) {
        return nullptr;
    }

    // TODO unpack building
    building.resource_id = uint16le(buf);
    building.tile_x = uint16le(buf + 2);
    building.tile_y = uint16le(buf + 4);
    building.train = uint32le(buf + 8);

    return &building;
}

SavegameTrain* Savegame::ReadTrain() {
    char buf[0x2c];
    is->read(buf, 0x2c);
    if (is->gcount() != 0x2c) {
        return nullptr;
    }
    // TODO copy fields
    return &train;
}

bool LoadSavegame(const std::string &path, bool unk1, bool unk2) {
    Savegame savegame;

    savegame.Open(path);

    if (!savegame.CheckVersion()) {
        savegame.Close();
        return false;
    }

    int32_t offset_x = (WORLD.tiles_x - savegame.header.tiles_x) / 2;
    int32_t offset_y = (WORLD.tiles_y - savegame.header.tiles_y) / 2;

    if (unk1) {
        // TODO WORLD.Clear();
    }

    if (path == "~curr") {
        // TODO
    }

    // TODO something

    for (size_t i = 0; i < savegame.header.building_count; i++) {
        SavegameBuilding* building = savegame.ReadBuilding();
        if (!building) {
            continue;
        }

        Entity* builing = WORLD.AddTile(building->resource_id, building->tile_x + offset_x, building->tile_y + offset_y, true, 1);


    }

    return true;
}
