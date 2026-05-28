#include "building_manager.h"
#include "track.h"
#include "depot.h"
#include "tunnel.h"
#include "globals.h"

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
            entity = new Tunnel(resource_id);
        } else if (asset->IsDepot()) {
            entity = new Depot(resource_id);
        } else {
            entity = new Track(resource_id);
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
    } else if (type == 3) {
        int32_t num_depots = 0;
        for (auto building : tiles) {
            uint32_t asset_type = building->asset ? building->asset->type : 0;
            if (asset_type == TRACK && static_cast<TrackAsset*>(building->asset)->IsDepot()) {
                num_depots++;
            }
        }
        if (num_depots > 0) {
            int32_t random_depot = (std::rand() % num_depots) + 1;
            int32_t current_depot = 0;
            for (auto building : tiles) {
                uint32_t asset_type = building->asset ? building->asset->type : 0;
                if (asset_type == TRACK && static_cast<TrackAsset*>(building->asset)->IsDepot()) {
                    current_depot++;
                    if (current_depot == random_depot) {
                        return building;
                    }
                }
            }
        }
    }
    return nullptr;
}
