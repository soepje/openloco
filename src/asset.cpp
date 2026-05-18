#include <cstdint>
#include <format>
#include <limits>
#include <sstream>
#include <string>

#include "asset.h"
#include "resource.h"
#include "strings.h"
#include "util.h"

// TODO i have a suspicion Asset was originally named Resource based on resource_id strings in data file

void inline read_uint8(std::istream &is, uint8_t& value) {
    uint16_t tmp;
    is >> tmp;
    value = tmp;
}

uint32_t GetType(uint32_t id_) {
    // TODO assemlby is a bit weird, maybe recheck?
    return id_ / 1024;
}

BaseAsset::BaseAsset(uint32_t id, const std::string& name) {
    BaseAsset::Load(id, name);
}

Bitmap* BaseAsset::LoadBitmap(uint32_t width, uint32_t height) {
    if (total_number_of_frames == 0) {
        return nullptr;
    }
    if (bitmap == nullptr) {
        bitmap = new Bitmap();
        bitmap->Load(image_name, 0, width, height);
    }
    if (bitmap->texture == nullptr) {
        delete bitmap;
        bitmap = nullptr;
        throw std::runtime_error("failed to load bitmap");
    }
    frame_width = bitmap->width / total_number_of_frames;
    frame_height = bitmap->height;
    instances++;
    if (!button_visible) {
        // TODO easter egg
    }
    for (size_t i = 0; i < number_of_frame_sets; i++) {
        // TODO AssetManager get sound
    }
    if (id == 2114) {
        // TODO clocktower
    }
    return bitmap;
}

void BaseAsset::ReleaseBitmap() {

}

void BaseAsset::Load(uint32_t id_, const std::string& name_) {
    id = id_;
    type = GetType(id_);

    total_number_of_frames = 1;

    // *(undefined4 *)((int)this + 0xc) = 0;
    // *(undefined4 *)((int)this + 0x38) = 0;
    // *(undefined4 *)((int)this + 0x3c) = 0;
    // *(undefined2 *)((int)this + 0x14) = 0;
    // *(undefined2 *)((int)this + 0x16) = 0;
    // *(undefined2 *)((int)this + 0x160) = 1;
    // *(undefined2 *)((int)this + 0x1a) = 0;
    // *(undefined2 *)((int)this + 0x1c) = 0;
    // *(undefined2 *)((int)this + 0x1e) = 0;
    // *(undefined2 *)((int)this + 0x28) = 0;
    // *(undefined2 *)((int)this + 0x2a) = 0;
    // *(undefined2 *)((int)this + 0x2c) = 0;
    // *(undefined4 *)((int)this + 0x40) = 0xffffffff;
    // *(undefined4 *)((int)this + 0x44) = 0xffffffff;
    // *(undefined1 *)((int)this + 0x18) = 0;
    // *(undefined1 *)((int)this + 0x163) = 1;
    // *(undefined1 *)((int)this + 0x162) = 0;
    // *(undefined1 *)((int)this + 0x14d) = 0;
    // *(undefined4 *)((int)this + 0x15c) = 0xffffffff;

    must_have = -1;
    cant_have = -1;

    max_instances = std::numeric_limits<uint32_t>::max();

    // this is a bit simplified
    if (!name_.empty()) {
        std::string dat_name = name_ + ".dat";
        image_name = name_ + ".bmp";

        size_t resource_size = 0;
        char* resource = ResourceManager::GetInstance()->Get(dat_name, resource_size);
        std::istringstream resource_stream(std::string(resource, resource_size));
        success = Parse(resource_stream);
        delete resource;
    }
}

bool BaseAsset::Parse(std::istream &is) {
    bool ok = true;
    std::string word;
    is >> word;
    while (word != "-9" && is.good()) {
        if (Compare(word, "button")) {
            is >> word; // skip "offset"
            is >> button_offset_x;
            is >> button_offset_y;
            is >> button_frames;
        } else if (Compare(word, "name")) {
            // is >> name;
            std::getline(is, name);
        } else if (Compare(word, "hotspot")) {
            is >> hotspot_x;
            is >> hotspot_y;
        } else if (Compare(word, "ShadowId")) {
            is >> shadow_id;
        } else if (Compare(word, "ShadowOffset")) {
            is >> shadow_offset_x;
            is >> shadow_offset_y;
        } else if (Compare(word, "animation")) {
            // unused
        } else if (Compare(word, "semi-transparent")) {
            flags |= 1 << 10;
        } else if (Compare(word, "shadows")) {
            flags |= 1 << 1;
        } else if (Compare(word, "must/cant_have")) {
            is >> must_have;
            is >> cant_have;
        } else if (Compare(word, "MaxInstances")) {
            is >> max_instances;
        } else if (Compare(word, "total_number_of_frames")) {
            is >> total_number_of_frames;
            if (total_number_of_frames == 0) {
                total_number_of_frames = 1;
            }
        } else if (Compare(word, "number_of_frame_sets")) {
            is >> number_of_frame_sets;
            if (number_of_frame_sets > 0) {
                frame_sets = new FrameSet[number_of_frame_sets];
            }
        } else if (Compare(word, "cursor_frame_set") || Compare(word, "cursor/default_frame_set")) {
            is >> cursor_frame_set;
            is >> default_frame_set;
            if (cursor_frame_set != -1 && cursor_frame_set >= number_of_frame_sets) {
                ok = false;
            }
            if (default_frame_set != -1 && default_frame_set >= number_of_frame_sets) {
                ok = false;
            }
            for (int i = 0; i < number_of_frame_sets; i++) {
                FrameSet* frame_set = &frame_sets[i];
                is >> word; // skip name
                is >> frame_set->start_frame;
                is >> frame_set->end_frame;
                is >> frame_set->animation_delay;
                read_uint8(is, frame_set->split_frames);
                is >> frame_set->restart_delay;
                is >> frame_set->next_frame_set;
                is >> frame_set->sound;
                is >> frame_set->replay_delay;
                is >> frame_set->field_14;
                read_uint8(is, frame_set->flip);

                if (frame_set->animation_delay == 0) {
                    frame_set->animation_delay = 1;
                }
                if (frame_set->start_frame == frame_set->end_frame && frame_set->next_frame_set == i) {
                    frame_set->next_frame_set = -1;
                }
                if (frame_set->start_frame >= total_number_of_frames || frame_set->end_frame >= total_number_of_frames) {
                    ok = false;
                }
                if (frame_set->next_frame_set >= number_of_frame_sets) {
                    ok = false;
                }
            }
        } else {
            // break;
            throw std::runtime_error("unknown token: " + word); // TODO remove
        }
        is >> word;
    }

    auto button_bitmap_name = std::string(image_name).replace(image_name.length() - 3, 3, "but");
    button_bitmap = new Bitmap();
    button_bitmap->Load(button_bitmap_name, 0, 0, 0);
    if (button_bitmap->texture == nullptr) {
        delete button_bitmap;
        button_bitmap = nullptr;
    }
    if (button_bitmap != nullptr && button_frames != 0) {
        button_frame_width = button_bitmap->width / button_frames;
        button_frame_height = button_bitmap->height;
    }

    return ok;
}

TileAsset::TileAsset(uint32_t id, const std::string& name_) : BaseAsset(id, "") {
    if (!name_.empty()) {
        std::string dat_name = name_ + ".dat";
        image_name = name_ + ".bmp";

        size_t resource_size = 0;
        char* resource = ResourceManager::GetInstance()->Get(dat_name, resource_size);
        std::istringstream is(std::string(resource, resource_size));
        if (TileAsset::Parse(is)) {
            success = BaseAsset::Parse(is);
        } else {
            success = false;
        }
        delete resource;
    }
}

bool TileAsset::Parse(std::istream &is) {
    bool ok = true;
    uint32_t minifigs = 0;

    // TODO reset fields

    delete insert_seq.seq;
    delete mobile_seq.seq;
    delete total_visits.seq;

    std::string token;
    is >> token;

    while (!Compare(token, "-9") && is.good()) {
        if (Compare(token, "physical_occupancy")) {
            ParsePhysicalOccupancy(is);
        } else if (Compare(token, "bitmap_occupancy")) {
            read_uint8(is, bitmap_occupancy_x);
            read_uint8(is, bitmap_occupancy_y);
            for (ssize_t y = 0; y < bitmap_occupancy_y; y++) {
                for (ssize_t x = 0; x < bitmap_occupancy_x; x++) {
                    read_uint8(is, bitmap_occupancy[x][y]);
                }
            }
        } else if (Compare(token, "entry_exit")) {
            ParseEntryExit(is);
        } else if (Compare(token, "RMBSeq")) {
            is >> rmb_seq;
        } else if (Compare(token, "ClosedFS")) {
            is >> closed_fs;
        } else if (Compare(token, "EEReplayDelay")) {
            is >> eer_replay_delay;
        } else if (Compare(token, "MaxMinifigForResource")) {
            read_uint8(is, this->max_minifig);
            if (this->max_minifig > 5) {
                this->max_minifig = 5;
            }
        } else if (Compare(token, "MaxMobileForResource")) { // loco originally used MaxM for both
            read_uint8(is, this->max_minifig);
            if (this->max_minifig > 5) {
                this->max_minifig = 5;
            }
        } else if (Compare(token, "LeisureDestination")) {
            read_uint8(is,this->leisure_destination);
        } else if (Compare(token, "MaxEmployees")) {
            read_uint8(is, this->max_employees);
            if (this->max_employees > 5) {
                this->max_employees = 5;
            }
        } else if (Compare(token, "PossibleEmployees")) {
            for (ssize_t i = 0; i < 5; i++) {
                is >> this->possible_employees[i];
                // TODO implement check

            }
        } else if (Compare(token, "PossibleMinifigs")) {
            for (ssize_t i = 0; i < 5; i++) {
                is >> this->possible_minifigs[i];
                // TODO implement check
                if (this->possible_minifigs[i] != -1) {
                    minifigs++;
                }
            }
        } else if (Compare(token, "shifts")) {
            is >> shift_start.hours;
            is >> shift_start.minutes;
            is >> shift_end.hours;
            is >> shift_end.minutes;
        } else if (Compare(token, "FreeToRoam")) {
            is >> free_to_roam.left;
            is >> free_to_roam.top;
            is >> free_to_roam.right;
            is >> free_to_roam.bottom;
        } else if (Compare(token, "ButtonVisible")) {
            read_uint8(is, button_visible);
        } else if (Compare(token, "InsertSeq")) {
            ParseSequence(is, insert_seq);
        } else if (Compare(token, "MobileSeq")) {
            ParseSequence(is, mobile_seq);
        } else if (Compare(token, "TotalVisits")) {
            ParseSequence(is, total_visits);
        } else {
            throw std::runtime_error("unknown token: " + token); // TODO remove
            // ok = false;
        }

        is >> token;
    }

    if (!Compare(token, "-9")) {
        ok = false;
    }

    total_height = physical_occupancy_y*16 + bitmap_occupancy_y*16;

    if (minifigs == 0) {
        max_minifig = 0;
    }

    return ok;
}

void TileAsset::ParsePhysicalOccupancy(std::istream &stream) {
    read_uint8(stream, physical_occupancy_x);
    read_uint8(stream, physical_occupancy_y);
    read_uint8(stream, physical_occupancy_z);

    std::fill_n( &physical_occupancy[0][0][0], sizeof(physical_occupancy), 0);

    for (ssize_t z = 0; z < physical_occupancy_z; z++) {
        for (ssize_t y = 0; y < physical_occupancy_y; y++) {
            for (ssize_t x = 0; x < physical_occupancy_x; x++) {
                read_uint8(stream, physical_occupancy[x][y][z]);
            }
        }
    }
}

void TileAsset::ParseEntryExit(std::istream &stream) {
    int16_t a, b, c, d;

    stream >> a;
    stream >> b;
    stream >> c;
    stream >> d;

    entry_exit[0] = Point{-1, -1};
    entry_exit[1] = Point{-1, -1};
    entry_exit[2] = Point{-1, -1};
    entry_exit[3] = Point{-1, -1};

    // TODO this needs some work
    if (a != 0) {
        if (a == 2) {
            entry_exit[0] = {0, int32_t{bitmap_occupancy_y} * 32 / 4};
        } else {
            entry_exit[0] = { 0, a};
        }
    }

    if (b != 0) {
        if (b == 2) {
            entry_exit[1] = {int32_t{bitmap_occupancy_x} * 32 / 4, int32_t{bitmap_occupancy_y} * 16 - 1};
        } else {
            entry_exit[1]  = {b, int32_t{bitmap_occupancy_y} * 16 - 1};
        }
    }

    if (c != 0) {
        if (c < 4) {
            entry_exit[2] = {int32_t{bitmap_occupancy_x} * 16 - 1, (int32_t{bitmap_occupancy_y} * c * 16) / 4};
        } else {
            entry_exit[2] = {int32_t{bitmap_occupancy_x} * 16 - 1, c};
        }
    }

    if (d != 0) {
        if (d < 4) {
            entry_exit[3] = {(int32_t{bitmap_occupancy_x} * d * 16) / 4, (int32_t{bitmap_occupancy_y} - physical_occupancy_y) * 16};
        } else {
            entry_exit[3] = {d, (int32_t{bitmap_occupancy_y} - physical_occupancy_y) * 16};
        }
    }
}

bool TileAsset::ParseSequence(std::istream &is, Sequence& sequence) {
    is >> sequence.field_0;
    is >> sequence.count;

    if (sequence.count > 0 and sequence.count < 45) {
        sequence.seq = new int32_t[sequence.count];
        for (size_t i = 0; i < sequence.count; i++) {
            is >> sequence.seq[i];
        }
    } else {
        sequence.count = 0;
    }

    // skip name
    std::string name;
    is >> name;

    is >> sequence.new_resource_id;
    is >> sequence.new_frameset;
    is >> sequence.minifig_resource_id;
    is >> sequence.minifig_frameset;
    is >> sequence.duration;
    is >> sequence.easter_egg_resource_id;
    is >> sequence.easter_egg_frameset;
    is >> sequence.action;
    is >> sequence.x;
    is >> sequence.y;

    // TODO add missing validation

    return true;
}

TrackAsset::TrackAsset(uint32_t id, const std::string& name) : TileAsset(id, name) {

}

bool TrackAsset::IsDepot() {
    return track_type == DEPOT_LEFT || track_type == DEPOT_TOP || track_type == DEPOT_BOTTOM || track_type == DEPOT_RIGHT;
}

bool TrackAsset::IsTunnel() {
    return track_type == TUNNEL_LEFT || track_type == TUNNEL_TOP || track_type == TUNNEL_BOTTOM || track_type == TUNNEL_RIGHT;
}

bool TrackAsset::IsStation() {
    return track_type == STATION_H || track_type == STATION_V;
}


bool TrackAsset::Parse(std::istream &is) {
    if (points != nullptr) {
        delete points;
        points = nullptr;
    }

    std::string word;
    is >> word; // skip "coords"

    int16_t points_len_tmp, points_alt_len_tmp;
    is >> points_len_tmp >> points_alt_len_tmp;

    points_x = points_len_tmp - 1;
    if (points_alt_len_tmp == 0) {
        points_y = 0;
    } else {
        points_y = points_alt_len_tmp + points_len_tmp - 1;
    }

    if (points_len_tmp > 0 || points_alt_len_tmp > 0) {
        points = new int16_t[(points_len_tmp + points_alt_len_tmp) * 2];
    }

    for (size_t i = 0; i < points_len_tmp; i++) {
        is >> points[i * 2];
        is >> points[i * 2 + 1];
    }

    int16_t terminator;
    is >> terminator; // skip -9

    for (size_t i = 0; i < points_alt_len_tmp; i++) {
        is >> points[(points_len_tmp + i) * 2];
        is >> points[(points_len_tmp + i) * 2 + 1];
    }

    is >> terminator;
    if (terminator != -9) {
        return false;
    }

    is >> word;
    while (is.good() && !word.empty()) {
        if (Compare(word, "tunnel")) {
            std::string direction;
            is >> direction;
            if (direction == "left") {
                track_type = TUNNEL_LEFT;
            } else if (direction == "right") {
                track_type = TUNNEL_RIGHT;
            } else if (direction == "top") {
                track_type = TUNNEL_TOP;
            } else if (direction == "bottom") {
                track_type = TUNNEL_BOTTOM;
            }
        } else if (Compare(word, "bridge")) {
            std::string direction;
            is >> direction;
            if (direction == "horizontal") {
                track_type = BRIDGE_HORIZONTAL;
            } else if (direction == "vertical") {
                track_type = BRIDGE_VERTICAL;
            }
        } else if (Compare(word, "depot")) {
            std::string direction;
            is >> direction;
            if (direction == "left") {
                track_type = DEPOT_LEFT;
            } else if (direction == "right") {
                track_type = DEPOT_RIGHT;
            } else if (direction == "top") {
                track_type = DEPOT_TOP;
            } else if (direction == "bottom") {
                track_type = DEPOT_BOTTOM;
            }
        } else if (Compare(word, "points")) {
            track_type = POINTS;
        } else if (Compare(word, "switch")) {
            track_type = SWITCH;
        } else if (Compare(word, "crosstrack")) {
            track_type = CROSSTRACK;
        } else if (Compare(word, "levelcrossing")) {
            std::string crossing_type;
            is >> crossing_type;
            if (crossing_type == "path-x-h") {
                track_type = PATH_X_H;
            } else if (crossing_type == "path-x-v") {
                track_type = PATH_X_V;
            } else if (crossing_type == "road-x-h") {
                track_type = ROAD_X_H;
            } else if (crossing_type == "road-x-v") {
                track_type = ROAD_X_V;
            }
        } else if (Compare(word, "station")) {
            std::string station_type;
            is >> station_type;
            if (station_type == "station-h") {
                track_type = STATION_H;
            } else if (station_type == "station-v") {
                track_type = STATION_V;
            }
        }

        is >> word;
    }

    return true;
}

TrainAsset::TrainAsset(uint32_t id, const std::string& name) : BaseAsset(id, name) {

}


bool TrainAsset::Parse(std::istream &is) {
    if (is.good()) {
        std::string word;
        is >> word; // skip "speed"
        is >> speed;
        is >> speed_reverse;
    }

    ParseTrainData();
}

bool TrainAsset::ParseTrainData() {
    size_t data_size = 0;
    char* data = ResourceManager::GetInstance()->Get("trains\\train.dat", data_size);
    if (data == nullptr) {
        return false;
    }

    std::istringstream is(data);

    for (size_t i = 0; i < 160; i++) {
        // TODO error handling

        is >> train_data[0][i][0];
        is >> train_data[0][i][1];
        is >> train_data[1][i][0];
        is >> train_data[1][i][1];
    }

    return true;
}

PersonAsset::PersonAsset(uint32_t id, const std::string& name_) : BaseAsset(id, "") {
    if (!name_.empty()) {
        std::string dat_name = name_ + ".dat";
        image_name = name_ + ".bmp";

        size_t resource_size = 0;
        char* resource = ResourceManager::GetInstance()->Get(dat_name, resource_size);
        std::istringstream resource_stream(std::string(resource, resource_size));
        if(Parse(resource_stream)) {
            success = BaseAsset::Parse(resource_stream);
        } else {
            success = false;
        }
        delete resource;
    }
}

bool PersonAsset::Parse(std::istream &is) {
    std::string word;
    is >> word;
    while (is.good() && word != "-9") {
        if (Compare(word, "walk_speed")) {
            read_uint8(is, walk_speed_a);
            read_uint8(is, walk_speed_b);
        } else if (Compare(word, "employable")) {
            read_uint8(is, employable);
        } else if (Compare(word, "sex")) {
            is >> word;
            sex = std::toupper(word[0]) == 'M' ? 'M' : 'F';
        } else if (Compare(word, "groundwidth")) {
            read_uint8(is, ground_width);
        } else if (Compare(word, "SpawnLimit")) {
            read_uint8(is, spawn_limit);
        } else if (Compare(word, "PickUpSoundId")) {
            is >> pickup_sound_id;
        }
        is >> word;
    }
    if (word != "-9") {
        return false;
    }
    return true;
}

bool AssetManager::LoadAsset(uint32_t id, const std::string& name) {
    // implementation is simplified

    if (id < 1024 || id >= 1024 * 16) {
        return false;
    }

    if (assets[id] != nullptr) {
        return true;
    }

    uint32_t type = id / 1024;
    BaseAsset* asset = nullptr;

    switch (type) {
        case 2:
        case 4:
            asset = (id % 2 == 0) ? new TileAsset(id, name) : new BaseAsset(id, name);
            break;
        case 3:
            asset = (id % 2 == 0) ? new TrackAsset(id, name) : new BaseAsset(id, name);
            break;
        case 5:
            asset = new BaseAsset(id, name);
            // TODO asset->is_cursor??? = true;
            break;
        case 6:
            if (id == 6146) {
                return false;
            }
            asset = (id < 6246 && id % 2 == 0) ? new TrainAsset(id, name) : new BaseAsset(id, name);
            break;
        case 7:
        case 8:
            asset = (id % 2 == 0) ? new PersonAsset(id, name) : new BaseAsset(id, name);
            break;
        case 12:
        case 13:
            asset = new TileAsset(id, name);
            break;
        case 14:
            asset = new BaseAsset(id, name);
            if (id > 14337) {
                // TODO asset->is_cursor??? = true;
            }
            break;
        default:
            asset = new BaseAsset(id, name);
            break;
    }

    if (!asset->success) {
        delete asset;
        return false;
    }

    assets[id] = asset;

    return true;
}

BaseAsset *AssetManager::GetAsset(uint32_t id) {
    if (id < 1024 || id >= 1024 * 16) {
        return nullptr;
    }

    if (assets[id] == nullptr) {
        LoadAsset(id, LoadString(id));
    }

    return assets[id];
}
