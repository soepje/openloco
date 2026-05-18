#include <cstdint>

#include "world.h"
#include "asset.h"
#include "entity.h"
#include "globals.h"
#include "openloco.h"
#include "building.h"

Entity *World::GetTile(int16_t x, int16_t y, int16_t z) {
    if (x >= 0 && x < 81 && y >= 0 && y < 65) {
        return grid[x][y].physical_occupancy[z];
    }
    return nullptr;
}

Entity *World::GetNeighborTile(Entity* entity, uint32_t direction) {
    Building* tile_entity = dynamic_cast<Building *>(entity);
    TileAsset* tile_asset = dynamic_cast<TileAsset *>(tile_entity->asset);

    if (tile_asset == nullptr) {
        return nullptr;
    }

    if (tile_asset->entry_exit[direction] == Point{0, 0}) {
        return nullptr;
    }

    Point entry_exit = {};
    tile_entity->GetEntryExit(&entry_exit, direction);

    int32_t tile_x = entry_exit.x >= 0 ? entry_exit.x / 16 : -1;
    int32_t tile_y = entry_exit.y >= 0 ? entry_exit.y / 16 : -1;

    switch (direction) {
        case 0:
            tile_x--;
            break;
        case 1:
            tile_y++;
            break;
        case 2:
            tile_x++;
            break;
        case 3:
            tile_y--;
            break;
        default:
            break;
    }

    Building* neighbor = nullptr;
    if (tile_x >= 0 && tile_x <= 80 && tile_y >= 0 && tile_y <= 64) {
        neighbor = grid[tile_y][tile_x].physical_occupancy[0];
    }

    if (neighbor != nullptr && neighbor->asset != nullptr) {
        Point p2 = {};
        neighbor->GetEntryExit(&p2, (direction - 2) % 4);
        if (Distance(entry_exit, p2) > 17) {
            return nullptr;
        }
    }

    return neighbor;
}

Entity* World::AddTile(uint32_t resource_id, int16_t x, int16_t y, bool no_offset, uint32_t unk2) {
    if (x < 0 || x >= tiles_x || y < 0 || y >= tiles_y) {
        return nullptr;
    }

    auto asset = dynamic_cast<TileAsset*>(ASSET_MANAGER.GetAsset(resource_id));
    if (asset == nullptr) {
        return nullptr;
    }

    int16_t offset_y = asset->bitmap_occupancy_y - asset->physical_occupancy_y;

    if (!no_offset) {
        y -= offset_y;
    }

    if (y < 0) {
        return nullptr;
    }

    // TODO some checks

    auto tile = dynamic_cast<Building*>(BUILDING_MANAGER.CreateBuilding(resource_id));
    if (!tile) {
        return nullptr;
    }

    for (size_t i = 0; i < asset->physical_occupancy_x; i++) {
        for (size_t j = 0; j < asset->physical_occupancy_y; j++) {
            for (size_t k = 0; k < asset->physical_occupancy_z; k++) {
                if (asset->physical_occupancy[i][j][k] == 1) {
                    grid[x+i][y+j+offset_y].physical_occupancy[k] = tile;
                    if (k > grid[x+i][y+j+offset_y].max_physical_z) {
                        grid[x+i][y+j+offset_y].max_physical_z = k;
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < asset->bitmap_occupancy_x; i++) {
        for (size_t j = 0; j < asset->bitmap_occupancy_y; j++) {
            uint8_t z = asset->bitmap_occupancy[i][j];
            if (z != 0) {
                z--;
                grid[x+i][y+j].bitmap_occupancy[z] = tile;
                if (z > grid[x+i][y+j].max_bitmap_z) {
                    grid[x+i][y+j].max_bitmap_z = z;
                }
                dirty[y * tiles_x + x] = true;
            }
        }
    }

    tile->tile_x = x;
    tile->tile_y = y;
    tile->SetViewPosition(x*16, y*16);

    return tile;
}

void World::UpdateDimensions() {
    // TODO
}

void World::Draw() {
    // TODO implement more efficient implementation
    dirty.set();
    Rect r = {0, 0, 1200, 800};
    BACKDROP->bitmap->Draw(r, nullptr, r, 1);
    DrawRect(r);
}

void World::DrawRect(const Rect& r) {
    int32_t start_tile_x = r.left < 0 ? -1 : r.left / 16;
    int32_t start_tile_y = r.top < 0 ? -1 : r.top / 16;
    int32_t end_tile_x = r.right < 0 ? -1 : r.right / 16;
    int32_t end_tile_y = r.bottom < 0 ? -1 : r.bottom / 16;

    for (int32_t x = start_tile_x; x < end_tile_x; x++) {
        for (int32_t y = start_tile_y; y < end_tile_y; y++) {
            if (x < 0 || x >= tiles_x || y < 0 || y >= tiles_y) {
                continue;
            }

            if (!dirty[y*tiles_x+x]) {
                continue;
            }

            Rect r2 = {
              x*16, y*16, (x+1)*16, (y+1)*16
            };

            const Tile& t = grid[x][y];
            int8_t max_bitmap_z = t.max_bitmap_z;
            if (max_bitmap_z < 2) {
                max_bitmap_z = 2;
            }

            for (int8_t z = 0; z <= max_bitmap_z; z++) {
                auto entity = t.bitmap_occupancy[z];
                if (entity != nullptr) {
                    entity->DrawFrame(r2, false, 0);
                }
                if (GAME_STATE == WORLD_VIEW) {
                    if (z == 0) {
                        if (t.bitmap_occupancy[z] != nullptr && t.bitmap_occupancy[z]->asset->type == TRACK) {
                            // TODO TRAIN_MANAGER.Draw();
                            MINIFIG_MANAGER.Draw(z, r2, false);
                        } else {
                            MINIFIG_MANAGER.Draw(z, r2, false);
                            // TODO TRAIN_MANAGER.Draw();
                        }
                    }

                    // TODO
                }
                if (entity != nullptr && entity->asset->frame_sets[entity->current_frameset].split_frames) {
                    entity->DrawSplitFrame(r2, false, 0);
                }
            }

            // TODO draw remaining stuff
        }
    }

    // TODO draw remaining stuff
}
