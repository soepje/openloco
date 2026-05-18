#include <cmath>
#include <cstdint>
#include <stdexcept>

#include "minifig.h"
#include "asset.h"
#include "globals.h"
#include "rect.h"
#include "util.h"
#include "strings.h"
#include "vehicle_minifig.h"
#include "walking_minifig.h"

Minifig::Minifig(uint32_t resource_id) : ImageEntity(resource_id, -1, 0, 0) {
    some_timestamp = EVENT_MANAGER.time;

    if (auto person_asset = dynamic_cast<PersonAsset*>(asset)) {
        if (person_asset->name.empty()) {
            uint32_t string_id;
            if (person_asset->sex == 'M') {
                string_id = (std::rand() % 49) + 2;
            } else {
                string_id = (std::rand() % 11) + 51;
            }
            name = LoadString(string_id);
        } else {
            ImageEntity::SetName(person_asset->name);

            // TODO
        }
    }
}

void Minifig::DrawFrame(const Rect &rect, bool unk, uint32_t flags) {
    ImageEntity::DrawFrame(rect, unk, flags);
}

void Minifig::SetName(const std::string& name) {
    ImageEntity::SetName(name);

    uint8_t type = (asset == nullptr ? 0 : asset->type);
    if (type == MINIFIG) {
        // TODO call something in RoamingEntityManager
    }

    if (Compare(name, "PARTY")) {
        // TODO
    }
}

void Minifig::Update(Minifig* follow) {
    ImageEntity::Update();
    if (some_timestamp2 != 0 && some_timestamp2 + 180 < EVENT_MANAGER.time) {
        if (mood != 0) {
            mood--;
        }

        some_timestamp3 = 0;
        visible = false;
    }
}

void Minifig::SetDestination(Point x) {
    throw std::runtime_error("should be implemented by subclass");
}

void Minifig::SetDestination(Entity* destination) {
    auto tile_entity = dynamic_cast<Building*>(destination);

    if (tile_entity == nullptr || !tile_entity->ok || !WORLD.field_52490) {
        field_cc.x = -1;
        field_cc.y = -1;
        if (visible) {
            Point p = {};
            SetTargetDistance(p, field_cc, 0);
            return;
        }
    }

    Rect dest = {};

    if (!tile_entity->GetRoamingDestination(dest) || !static_cast<TileAsset*>(tile_entity->asset)->leisure_destination) {
        SetDestination(tile_entity->hotspot);
        return;
    }

    if (dest.right < dest.left) {
        std::swap(dest.left, dest.right);
    }

    int32_t dest_x = dest.right;
    if (dest.left - dest.right != -1) {
        dest_x = (std::rand() % ((dest.right - dest.left) + 1)) + dest.left;
    }

    if (dest.bottom < dest.top) {
        std::swap(dest.top, dest.bottom);
    }

    int32_t dest_y = dest.bottom;
    if (dest.top - dest.bottom != -1) {
        dest_y = (std::rand() % ((dest.bottom - dest.top) + 1)) + dest.top;
    }

    SetDestination(Point{dest_x, dest_y});
}

void Minifig::RoamingUnk4(Point p) {
    throw std::runtime_error("should be implemented by subclass");
}


Point& Minifig::SetTargetDistance(Point &point, Point p2, int32_t limit) {
    if (p2 == Point{-1, -1}) {
        target_distance_x = 0;
        target_distance_y = 0;
        target_distance = 0;
        point = hotspot;
        return point;
    }

    target_distance_x = p2.x - hotspot.x;
    target_distance_y = p2.y - hotspot.y;
    target_distance = std::sqrt(target_distance_x*target_distance_x + target_distance_y * target_distance_y);

    auto a = std::abs(target_distance_x);
    if (a > limit) {
        a = limit;
    }
    point.x = target_distance_x < 0 ? view.left - a : view.left + a;

    auto b = std::abs(target_distance_y);
    if (b > limit) {
        b = limit;
    }
    point.y = target_distance_y < 0 ? view.top - b : view.top + b;
    return point;
}

void Minifig::RoamingUnk5() {
    auto dist = Distance(hotspot, field_a8);

    // parameters still unkown
    // TODO reverse when this is called somewhere
}

void Minifig::SetWork() {
     if (auto work_tile_entity = dynamic_cast<Building*>(work)) {
         for (size_t i = 0; i < 5; i++) {
             if (work_tile_entity->employees[i] == work_tile_entity) {
                 work_tile_entity->employees[i] = nullptr;
                 work_tile_entity->employee_count--;
                 break;
             }
         }
         work = nullptr;
     }

    for (auto tile_entity : BUILDING_MANAGER.tiles) {
        if (tile_entity == nullptr) {
            continue;
        }

        auto tile_asset = dynamic_cast<TileAsset*>(tile_entity->asset);
        if (!tile_asset) {
            continue;
        }

        if (tile_entity->employee_count >= tile_asset->max_employees) {
            continue;
        }

        int32_t resource_id = asset == nullptr ? -1 : static_cast<int32_t>(asset->id);
        bool possible_employee = false;
        for (size_t i = 0; i < 5; i++) {
            if (tile_asset->possible_employees[i] == resource_id) {
                possible_employee = true;
                break;
            }
        }

        if (!possible_employee) {
            continue;
        }

        int32_t empty_slot = -1;
        for (size_t i = 0; i < 5; i++) {
            if (tile_entity->employees[i] == nullptr) {
                empty_slot = i;
                break;
            }
        }

        if (empty_slot >= 0) {
            tile_entity->employees[empty_slot] = this;
            tile_entity->employee_count++;
            work = tile_entity;
            break;
        }
    }
}

bool Minifig::IsValidTarget(Point x) {
    return false;
}

Minifig *MinifigManager::CreateMinifig(uint32_t resource_id, Entity *home, int32_t x, int32_t y) {
    auto minifig_asset = ASSET_MANAGER.GetAsset(resource_id);
    auto must_have_asset = ASSET_MANAGER.GetAsset(minifig_asset->must_have);
    auto cant_have_asset = ASSET_MANAGER.GetAsset(minifig_asset->cant_have);

    if (minifig_asset->must_have != -1) {
        if (must_have_asset == nullptr) {
            return nullptr;
        }

        if (must_have_asset->instances == 0) {
            return nullptr;
        }
    }

    if (cant_have_asset && cant_have_asset->instances > 0) {
        return nullptr;
    }

    auto entity_type = GetType(resource_id);
    if (entity_type == VEHICLE) {
        VehicleMinifig* vehicle = new VehicleMinifig(resource_id);
        if (vehicle == nullptr) {
            return nullptr;
        }

        if (!vehicle->ok) {
            delete vehicle;
            return nullptr;
        }

        vehicle->home = home;
        vehicle->SetViewPosition(x, y);
        vehicles.push_back(vehicle);
        vehicle_count++;
        return vehicle;
    } else if (entity_type == MINIFIG) {
        WalkingMinifig* minifig = new WalkingMinifig(resource_id);
        if (minifig == nullptr) {
            return nullptr;
        }

        if (!minifig->ok) {
            delete minifig;
            return nullptr;
        }

        minifig->home = home;
        minifig->SetViewPosition(x, y);
        minifigs.push_back(minifig);
        minifig_count++;
        return minifig;
    }

    return nullptr;
}

void MinifigManager::Draw(int32_t z, Rect r, bool unk) {
    if (z > 0) {
        return;
    }

    for (auto vehicle : vehicles) {
        // TODO weird break
        vehicle->DrawFrame(r, unk, 0);
    }

    for (auto minifig : minifigs) {
        minifig->DrawFrame(r, unk, 0);
    }
}

void MinifigManager::Update() {
    if (party && party_started + 300 < EVENT_MANAGER.time) {
        party = false;
    }

    for (size_t i = 0; i < minifigs.size(); i++) {
        auto next = i+1 >= minifigs.size() ? nullptr : minifigs[i+1];
        minifigs[i]->Update(next);
    }

    for (size_t i = 0; i < vehicles.size(); i++) {
        auto next = i+1 >= vehicles.size() ? nullptr : vehicles[i+1];
        vehicles[i]->Update(next);
    }

    // TODO do something with vehicles
}
