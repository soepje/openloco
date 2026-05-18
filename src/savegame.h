#ifndef OPENLOCO_SAVEGAME_H
#define OPENLOCO_SAVEGAME_H

#include <cstdint>
#include <istream>
#include <string>

struct SavegameHeader {
    uint16_t version; // 0
    uint16_t tiles_x; // 2
    uint16_t tiles_y; // 4
    uint32_t building_count; // 8
    uint16_t train_count; // c
    std::string backdrop; // e
};

struct SavegameBuilding {
    uint16_t resource_id; // 0
    uint16_t tile_x; // 2
    uint16_t tile_y; // 4
    uint32_t train; // 8
};

struct SavegameTrain {
    uint32_t segment_resource_ids[4]; // 0
};

class Savegame {
public:
    SavegameBuilding building; // 4
    SavegameTrain train; // 84
    SavegameHeader header; // b0
    uint8_t* thumbnail; // 1c4
    std::istream* is; // 1c8

    void Open(const std::string& path);
    void Close();
    bool CheckVersion();
    SavegameBuilding* ReadBuilding();
    SavegameTrain* ReadTrain();
};

bool LoadSavegame(const std::string& path, bool unk1, bool unk2);

#endif //OPENLOCO_SAVEGAME_H
