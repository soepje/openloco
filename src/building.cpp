#include "building.h"
#include "asset.h"
#include "minifig.h"
#include "time.h"
#include "globals.h"
#include <cstdint>
#include <cstdlib>

bool Building::GetRoamingDestination(Rect &dest) {
    if (auto tile_asset = dynamic_cast<TileAsset*>(asset)) {
        SetRectEmpty(dest);
        if (!IsRectEmpty(tile_asset->free_to_roam)) {
            dest = tile_asset->free_to_roam;
            OffsetRect(dest, view.left, view.top);
            return true;
        }
    }
    return false;
}

Building::Building(uint32_t asset_id) : ImageEntity(asset_id, -1, 0, 0) {
    type = 3;

    if (asset == nullptr) {
        return;
    }

    auto tile_asset = dynamic_cast<TileAsset*>(asset);
    if (tile_asset->max_minifig == 0) {
        max_resident_count = 0;
    } else {
        max_resident_count = (std::rand() % tile_asset->max_minifig) + 1;
    }

    ImageEntity::SetName(asset->name);

    if (asset->frame_sets[current_frameset].restart_delay >= 0) {
        restart_timestamp = EVENT_MANAGER.time + (std::rand() % 61);
    }
}

Entity* Building::AddMinifig(int32_t resource_id) {
    if (!ok) {
        return nullptr;
    }

    if (max_resident_count == 0) {
        return nullptr;
    }

    if (MINIFIG_MANAGER.minifig_count + MINIFIG_MANAGER.vehicle_count >= 100) {
        return nullptr;
    }

    auto tile_asset = dynamic_cast<TileAsset*>(asset);
    if (tile_asset == nullptr) {
        return nullptr;
    }

    Rect spawn_area = {};
    SetRectEmpty(spawn_area);
    if (!IsRectEmpty(tile_asset->free_to_roam)) {
        spawn_area = tile_asset->free_to_roam;
        OffsetRect(spawn_area, view.left, view.top);
    }

    size_t empty_minifig_slot = -1;
    for (size_t i = 0; i < max_resident_count; i++) {
        if (residents[i] == nullptr) {
            empty_minifig_slot = i;
            break;
        }
    }

    if (empty_minifig_slot < 0) {
        return nullptr;
    }

    // TODO What about the -1 entries in possible minifig???
    if (resource_id < 1) {
        resource_id = tile_asset->possible_minifigs[std::rand() % 5];
    }

    auto minifig_asset = dynamic_cast<PersonAsset*>(ASSET_MANAGER.GetAsset(resource_id));
    if (minifig_asset == nullptr) {
        return nullptr;
    }

    if (minifig_asset->type == MINIFIG && minifig_asset->instances < minifig_asset->max_instances) {
        // TODO lock

        int32_t spawn_x, spawn_y = 0;

        spawn_x = hotspot.x - minifig_asset->hotspot_x;
        spawn_y = hotspot.y - minifig_asset->hotspot_y;

        // TODO implement randomize spawn position


        Minifig* minifig = MINIFIG_MANAGER.CreateMinifig(resource_id, this, spawn_x, spawn_y);
        if (minifig) {
            if (minifig->IsValidTarget(hotspot)) {
                residents[empty_minifig_slot] = minifig;
                minifig->SetWork(); // ???
                resident_count++;
                return minifig;
            } else {
                // TODO RemoveEntity
            }

            // TODO call stuff in MovingEntity (not reversed yet)
        }

        // TODO unlock

        return nullptr;
    }

    if (minifig_asset->type == VEHICLE && minifig_asset->instances < minifig_asset->max_instances) {
        // TODO
    }

    return nullptr;
}

void Building::UnkTileEntity1() {
    SetSomething(current_frameset);
}

bool Building::UnkTileEntity2() {
    return false;
}

Point *Building::GetEntryExit(Point *point, uint32_t direction) {
    auto* tile_asset = dynamic_cast<TileAsset *>(asset);
    if (tile_asset == nullptr) {
        point->x = -1;
        point->y = -1;
        return point;

    }

    if (tile_asset->entry_exit[direction] == Point{-1, -1}) {
        point->x = -1;
        point->y = -1;
        return point;
    }

    point->x = view.left + tile_asset->entry_exit[direction].x;
    point->y = view.top + tile_asset->entry_exit[direction].y;

    return point;
}

void BuildingManager::Update() {
    for (auto entity : tiles) {
        entity->Update();
    }
}

Entity* BuildingManager::CreateBuilding(uint32_t resource_id) {
    Building* entity = nullptr;
    if (GetType(resource_id) == TRACK) {
        auto asset = static_cast<TrackAsset*>(ASSET_MANAGER.GetAsset(resource_id));
        if (asset->IsTunnel()) {
            entity = new TunnelEntity(resource_id);
        } else if (asset->IsDepot()) {
            entity = new DepotEntity(resource_id);
        } else {
            entity = new TrackEntity(resource_id);
        }
    } else {
        entity = new Building(resource_id);
    }

    if (!entity->ok) {
        delete entity;
        return nullptr;
    }

    tiles.push_back(entity);
    if (static_cast<TileAsset*>(entity->asset)->leisure_destination != 0) {
        leisure_destinations++;
    }
    tile_count++;

    return entity;
}

void BuildingManager::RemoveTile(Entity *entity, bool trigger_explosion_effect) {
    if (entity == nullptr) {
        return;
    }

    for (size_t i = 0; i < tiles.size(); i++) {
        if (tiles[i] == entity) {
            tiles.erase(tiles.begin()+i);
            tile_count--;
            if (static_cast<TileAsset*>(static_cast<ImageEntity*>(entity)->asset)->leisure_destination) {
                leisure_destinations--;
            }
            break;
        }
    }

    auto asset = dynamic_cast<TileAsset*>(static_cast<ImageEntity*>(entity)->asset);
    uint32_t type = asset == nullptr ? 0 : asset->type;

    if (type == PATH) {
        int32_t resource_id = asset == nullptr ? -1 : asset->id;
        // train stations?
        if (resource_id > 12304) {
            // TODO check i do not understand

            entity->ok = false;
            for (size_t i = 0; i < 4; i++) {
                auto neighbor = dynamic_cast<Building*>(WORLD.GetNeighborTile(entity, i));
                if (neighbor != nullptr && neighbor->ok) {
                    auto neighbor_type = neighbor->asset == nullptr ? -1 : neighbor->asset->type;
                    if (neighbor_type == TRACK) {
                        auto neighbor_asset = dynamic_cast<TrackAsset*>(neighbor->asset);
                        if (neighbor_asset != nullptr && neighbor_asset->IsStation()) {
                            neighbor->path_neighbors[0] = nullptr;
                            neighbor->path_neighbors[1] = nullptr;
                            neighbor->path_neighbors[2] = nullptr;
                            neighbor->path_neighbors[3] = nullptr;
                            // TODO GameManager_00455ab0(&GAME_MANAGER,puVar4,param_2);
                            break;
                        }
                    }
                }
            }
        }
    }

    auto track_asset = dynamic_cast<TrackAsset*>(static_cast<ImageEntity*>(entity)->asset);
    if (track_asset != nullptr && track_asset->type == TRACK && track_asset->IsStation()) {
        entity->ok = false;

        for (size_t i = 0; i < 4; i++) {
            auto neighbor = dynamic_cast<Building*>(WORLD.GetNeighborTile(entity, i));
            if (neighbor != nullptr && neighbor->ok) {
                auto neighbor_type = neighbor->asset == nullptr ? -1 : neighbor->asset->type;
                if (neighbor_type == PATH) {
                    if (neighbor->asset->id > 12304) {
                        // TODO check i do not understand

                        neighbor->path_neighbors[0] = nullptr;
                        neighbor->path_neighbors[1] = nullptr;
                        neighbor->path_neighbors[2] = nullptr;
                        neighbor->path_neighbors[3] = nullptr;
                        // GameManager_00455ab0(&GAME_MANAGER,puVar4,param_2);
                    }
                }
            }
        }
    }

    if (trigger_explosion_effect) {
        // TODO
    }

    delete entity;
}

Building* BuildingManager::GetRandomBuilding(int32_t type) {
    if (type == 2) {
        if (leisure_destinations > 0) {
            int32_t r = (std::rand() % leisure_destinations) + 1;
            int32_t c = 0;
            for (auto building : tiles) {
                if (static_cast<TileAsset*>(building->asset)->leisure_destination) {
                    c++;
                    if (c == r) {
                        return building;
                    }
                }
            }
        }
    }
    return nullptr;
}
