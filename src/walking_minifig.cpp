#include "walking_minifig.h"
#include "asset.h"
#include "event.h"
#include "globals.h"
#include "building.h"
#include "rect.h"
#include <cstdint>
#include <cstdlib>
#include <ctime>

WalkingMinifig::WalkingMinifig(uint32_t resource_id) : Minifig(resource_id) {
    mood = 4;
}

void WalkingMinifig::Update(Minifig* follow) {
    if (field_89 != 0) {
        return;
    }

    Minifig::Update(follow);

    if (!MINIFIG_MANAGER.party || follow == nullptr) {
        if (target_distance == 0) {
            if (some_timestamp3 < EVENT_MANAGER.time) {
                current_action = GetTimeAction(EVENT_MANAGER.time);
                if (current_action == 1) {
                    SetDestination(home);
                } else if (current_action == 2) {
                    SetDestination(work);
                } else if (current_action == 3) {
                    auto random_building = BUILDING_MANAGER.GetRandomBuilding(2);
                    if (random_building) {
                        if (random_building->hotspot == field_c4) {
                            SetDestination(random_building->hotspot);
                        }
                    }
                    if (field_cc == Point{-1, -1}) {
                        some_timestamp3 = (std::rand() % 21) + 10 + EVENT_MANAGER.time;
                    }
                }
            }
        } else {
            if (Move() == 0) {
                if (hotspot == field_a8) {
                    HandleTimeAction(current_action);
                } else {
                    RoamingUnk4(hotspot);
                }
            }
        }
    } else {
        Follow(follow);
    }

    if (target_distance == 0 && visible) {
        SetMoodFrameset();
    }
}

int32_t WalkingMinifig::Move() {
    if (field_f0 == 0) {
        field_e4++;
        if (visible && dynamic_cast<PersonAsset*>(asset)->walk_speed_b <= field_e4) {
            field_e4 = 0;

            Point p;
            SetTargetDistance(p, field_cc, dynamic_cast<PersonAsset*>(asset)->walk_speed_a);

            if (IsValidTarget(p)) {
                if (!WaitForTrain()) {
                    SetDirectionFrameset();
                    SetViewPosition(p.x, p.y);
                    field_e0++;
                    if (field_e0 > 10) {
                        field_98 = hotspot;
                        field_e0 = 0;
                    }
                    return target_distance;
                }
            }

            SetMoodFrameset();
            return target_distance;
        }
    } else {
        // TODO train stuff
    }
    return target_distance;
}

bool WalkingMinifig::Follow(Minifig* follow) {
    field_e4++;

    if ((visible || field_f0 != 0) && field_e4 > 2) {
        field_e4 = 0;
        visible = true;

        if (follow->field_98 == Point{-1, -1}) {
            // TODO probably
        }
    }

    return true;
}

bool WalkingMinifig::IsValidTarget(Point p) {
    bool ret = true;

    if (field_cc != field_b0 && field_cc != field_a8) {
        if (CURSOR_ENTITY.minifig != this) {
            return true;
        }
        if (!CURSOR_ENTITY.minifig_drag) {
            return true;
        }
    }

    // Turned this into a loop
    auto frame_width = static_cast<PersonAsset*>(asset)->frame_width;
    auto frame_height = static_cast<PersonAsset*>(asset)->frame_height;

    Point ps[2] = {
        Point{ p.x + 4, p.y + ((frame_height / 2) - 1) },
        Point{ p.x + (frame_width - 4), p.y + (frame_height - 4) }, // What is this???
    };

    for (auto p1 : ps) {
        int16_t tile_x = p1.x < 0 ? -1 : p1.x / 16; // TODO this code is repeated often, maybe move to utility?
        int16_t tile_y = p1.y < 0 ? -1 : p1.y / 16;

        Building* building = static_cast<Building*>(WORLD.GetTile(tile_x, tile_y, 0));
        if (building) {
            int32_t building_type = building->asset ? building->asset->type : 0;
            if (building_type >= 2 && building_type <= 4) {
                Rect roaming_dest;
                building->GetRoamingDestination(roaming_dest);
                if (!PtInRect(roaming_dest, p1)) {
                    return false;
                }
            } else if (building_type != 12) {
                return false;
            }
        }
    }

    return true;
}

int32_t WalkingMinifig::GetTimeAction(time_t t) {
    if (t < some_timestamp3) {
        return 0;
    }

    if (CURSOR_ENTITY.minifig == this && CURSOR_ENTITY.minifig_drag) {
        return 0;
    }

    if (!visible) {
        visible = true;
    }

    Timestamp ts(t);

    if (work) {
        auto tile_asset = static_cast<TileAsset*>(static_cast<Building*>(work)->asset);
        if (ts.Between(tile_asset->shift_start, tile_asset->shift_end)) {
            return 2;
        }
    }

    if (home) {
        auto tile_asset = static_cast<TileAsset*>(static_cast<Building*>(home)->asset);
        if (!ts.Between(tile_asset->shift_start, tile_asset->shift_end)) {
            return 1;
        }
    }

    return 3;
}

bool WalkingMinifig::HandleTimeAction(int32_t action) {
    if (!visible || (CURSOR_ENTITY.minifig == this && CURSOR_ENTITY.minifig_drag)) {
        return false;
    }

    some_timestamp2 = 0;

    if (action == 1) {
        if (hotspot == static_cast<Building*>(home)->hotspot) {
            if (mood < 7) {
                mood++;
            }
            visible = false;
        }
        some_timestamp3 = (std::rand() % 21) + 10 + EVENT_MANAGER.time;
    } else if (action == 2) {
        if (work) {
            if (hotspot == static_cast<Building*>(work)->hotspot) {
                if (mood < 7) {
                    mood++;
                }
                visible = false;
                some_timestamp3 = EVENT_MANAGER.time + 3600;
            } else {
                some_timestamp3 = (std::rand() % 21) + 10 + EVENT_MANAGER.time;
            }
        }
    } else if (action == 3) {
        if (mood < 7) {
            mood+=2;
        }
        if (!MINIFIG_MANAGER.party) {
            some_timestamp3 = (std::rand() % 21) + 10 + EVENT_MANAGER.time;
        }
    }

    field_c4 = field_a8;
    field_a8 = Point{-1, -1};

    return true;
}

void WalkingMinifig::SetDirectionFrameset() {
    if (abs(target_distance_y) < abs(target_distance_x)) {
        if (target_distance_x < 1) {
            if (current_frameset != 0) {
                SetSomething(0);
            }
        } else {
            if (current_frameset != 2) {
                SetSomething(2);
            }
        }
    } else {
        if (target_distance_y < 1) {
            if (current_frameset != 3) {
                SetSomething(3);
            }
        } else {
            if (current_frameset != 1) {
                SetSomething(1);
            }
        }
    }
}

void WalkingMinifig::SetMoodFrameset() {
    if (mood == 0) {
        if (current_frameset != 5) {
            SetSomething(5);
        }
    } else if (mood < 3) {
        if (current_frameset != 6) {
            SetSomething(6);
        }
    } else if (mood < 5) {
        if (current_frameset != 4) {
            SetSomething(4);
        }
    } else {
        if (current_frameset != 7) {
            SetSomething(7);
        }
    }
}

bool WalkingMinifig::WaitForTrain() {
    // TODO train stuff
    return false;
}
