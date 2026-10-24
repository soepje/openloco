#include "train_bogie.h"

#include <cstdint>

#include "track.h"
#include "train.h"
#include "asset.h"
#include "openloco.h"
#include "globals.h"
#include "building.h"
#include "train_car.h"
#include "tunnel.h"

TrainBogie::TrainBogie(bool tunnel) {
    direction = 1;
    point = 0;
    x = -1;
    y = -1;
    track = nullptr;
    if (tunnel) {
        tunnel_state = 2;
        depot_state = 0;
    } else {
        tunnel_state = 0;
        depot_state = 2;
    }
}

void TrainBogie::CheckDepot() {
    if (depot_state == 1) {
        TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);
        switch (track_asset->track_type) {
            case DEPOT_LEFT:
                if (x < ((track_asset->bitmap_occupancy_x + track->tile.x) * 16 - 16)) {
                    depot_state = 2;
                }
                break;
            case DEPOT_RIGHT:
                if ((track->tile.x + 1) * 16 < x) {
                    depot_state = 2;
                }
                break;
            case DEPOT_TOP:
                if (y < ((track_asset->bitmap_occupancy_y + track->tile.y) * 16 - 16)) {
                    depot_state = 2;
                }
                break;
            case DEPOT_BOTTOM:
                if ((track->tile.y + 1) * 16 < y) {
                    depot_state = 2;
                }
                break;
            default:
                break;
        }
    }
}

void TrainBogie::CheckTunnel() {
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);
    switch (track_asset->track_type) {
        case TUNNEL_LEFT:
            if (x < 1) {
                tunnel_state = 2;
            }
            break;
        case TUNNEL_RIGHT:
            if (x > WORLD.screen_width) {
                tunnel_state = 2;
            }
            break;
        case TUNNEL_TOP:
            if (y < 1) {
                tunnel_state = 2;
            }
            break;
        case TUNNEL_BOTTOM:
            if (y >= WORLD.screen_height) {
                tunnel_state = 2;
            }
            break;
    }
}

bool TrainBogie::CheckStation(Train* train) {
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);
    TrackAssetType track_type = track_asset->track_type;
    if (track_type == STATION_H || track_type == STATION_V) {
        if (train->station_timer == 1) {
            train->station_timer = 0;
            return false;
        }

        if (train->HasPassengerCar()) {
            if ((direction == 0 && point == 1) || (direction == 1 && point == (track_asset->num_points - 1))) {
                train->station_timer = 200;
                train->SetState(TrainState::WAITING);
                return true;
            }
        }
    }
    return false;
}

bool TrainBogie::SetTrack(int32_t x, int32_t y) {
    int16_t tile_x = (x < 0) ? -1 : x / 16;
    int16_t tile_y = (y < 0) ? -1 : y / 16;

    Building* building = dynamic_cast<Building*>(WORLD.GetBuilding(tile_x, tile_y, 0));
    if (!building) {
        return false;
    }

    if (GetType(building->asset->id) != TRACK) {
        return false;
    }

    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(building->asset);

    direction = 1;

    // TODO weird stuff (is this even necessary???)

    for (size_t i = 0; i < track_asset->num_points; i++) {
        if ((x - building->tile.x*16) == track_asset->points[i*2]) {
            point = i;
            break;
        }
    }

    x = track_asset->points[point * 2] + building->tile.x * 16;
    y = track_asset->points[point * 2 + 1] + building->tile.y * 16;

    return true;
}

bool TrainBogie::UpdateTrackConfig(Train* train, Track* track) {
    if (track->num_trains > 0) {
        train->SetState(TrainState::WAITING);
    }

    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);

    if ((direction != 1 || point != 0) &&
        (direction != 0 || point != track_asset->num_points) &&
        (direction != 1 || point != track_asset->num_points + 1) &&
        (direction != 0 || point != track_asset->num_points_alt)) {
        if (track->track_state == TrackState::CONFIG_A) {
            track->track_state = TrackState::CONFIG_B;
        } else if (track->track_state == TrackState::CONFIG_B) {
            track->track_state = TrackState::CONFIG_A;
        }

        if (!UpdateTrack()) {
            if (track->track_state == TrackState::CONFIG_A) {
                track->SetSomething(1);
            } else if (track->track_state == TrackState::CONFIG_B) {
                track->SetSomething(0);
            }
            return true;
        }

        if (track->track_state == TrackState::CONFIG_A) {
            track->track_state = TrackState::CONFIG_B;
            return false;
        } else if (track->track_state == TrackState::CONFIG_B) {
            track->track_state = TrackState::CONFIG_A;
            return false;
        }
    }

    return false;
}

Track* TrainBogie::UpdateTrack() {
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);

    int16_t next_x = track->tile.x * 16 + track_asset->points[point*2];
    int16_t next_y = track->tile.y * 16 + track_asset->points[point*2 + 1];

    if (next_x < 0) {
        return track;
    }

    if (next_y < 0) {
        return track;
    }

    Track* next_track = dynamic_cast<Track*>(WORLD.GetBuilding(next_x / 16, next_y / 16, 0));

    // TODO check all offsets into points buffer
    if (next_track) {
        TrackAsset* next_track_asset = dynamic_cast<TrackAsset*>(next_track->asset);

        if (next_track->track_state != TrackState::CONFIG_A || next_track_asset->track_type == CROSSTRACK) {
            if (next_x == next_track_asset->points[2] + next_track->tile.x*16 &&
                next_y == next_track_asset->points[3] + next_track->tile.y*16) {
                track->num_trains--;
                track = next_track;
                direction = 1;
                point = 1;
                track->num_trains++;
                return nullptr;
            }

            if (next_x == next_track_asset->points[next_track_asset->num_points*2-2] + next_track->tile.x*16 &&
                next_y == next_track_asset->points[next_track_asset->num_points*2-1] + next_track->tile.y*16) {
                if (next_track_asset->track_type == POINTS && next_track->track_state == TrackState::CONFIG_A) {
                    if (next_track_asset->points[0] != next_track_asset->points[next_track_asset->num_points_alt * 2 + 2] ||
                        next_track_asset->points[1] != next_track_asset->points[next_track_asset->num_points_alt * 2 + 3]) {
                        return track;
                    }
                }
                track->num_trains--;
                track = next_track;
                direction = 0;
                point = next_track_asset->num_points - 1;
                track->num_trains++;
                return nullptr;
            }
        }
        if (next_track_asset->num_points_alt != 0 &&
            next_x == next_track_asset->points[4 + next_track_asset->num_points * 2] + next_track->tile.x*16 &&
            next_y == next_track_asset->points[5 + next_track_asset->num_points * 2] + next_track->tile.y*16) {
            if (next_track_asset->track_type == POINTS && next_track->track_state == TrackState::CONFIG_B) {
                if (next_track_asset->points[0] != next_track_asset->points[next_track_asset->num_points_alt * 2 + 2] ||
                    next_track_asset->points[1] != next_track_asset->points[next_track_asset->num_points_alt * 2 + 3]) {
                    return next_track;
                }
            }
            track->num_trains--;
            track = next_track;
            direction = 1;
            point = next_track_asset->num_points + 2;
            track->num_trains++;
            return nullptr;
        }
        if (next_track_asset->num_points_alt == 0 ||
            next_x != next_track_asset->points[next_track_asset->num_points_alt*2-2] + next_track->tile.x*16 ||
            next_y != next_track_asset->points[next_track_asset->num_points_alt*2-1] + next_track->tile.y*16) {
            return next_track;
        }
        if (next_track_asset->track_type == POINTS && next_track->track_state == TrackState::CONFIG_B) {
            if (next_track_asset->points[0] != next_track_asset->points[next_track_asset->num_points_alt * 2 + 2] ||
                next_track_asset->points[1] != next_track_asset->points[next_track_asset->num_points_alt * 2 + 3]) {
                return next_track;
            }
        }
        track->num_trains--;
        track = next_track;
        direction = 0;
        point = next_track_asset->num_points_alt - 1;
        track->num_trains++;
        return nullptr;
    }

    return track;
}

bool TrainBogie::UpdatePoint(int32_t x, int32_t y) {
    if (!track) {
        return false;
    }

    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);

    if (x == track->tile.x*16 + track_asset->points[0]) {
        if (y != track->tile.y*16 + track_asset->points[1]) {
            if (track_asset->num_points != 0) {
                int32_t i = 0;
                while (y - track->tile.y*16 != track_asset->points[i*2+1]) {
                    i++;
                    if (i <= track_asset->num_points) {
                        return false;
                    }
                }
                point = i;
                y = track_asset->points[i*2+1] + track->tile.y*16;
                return true;
            }
        }
    } else {
        if (track_asset->num_points != 0) {
            int32_t i = 0;
            while (x - track->tile.x*16 != track_asset->points[i*2]) {
                i++;
                if (i <= track_asset->num_points) {
                    return false;
                }
            }
            point = i;
            x = track_asset->points[i*2] + track->tile.x*16;
            return true;
        }
    }

    return false;
}

bool TrainBogie::ExitDepot() {
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);
    switch (track_asset->track_type) {
        case DEPOT_LEFT:
            x++;
            if (x > (track->tile.x + track_asset->bitmap_occupancy_x - 1)*16) {
                UpdatePoint(x, y);
                depot_state = 0;
            }
            break;
        case DEPOT_RIGHT:
            x--;
            if (x < (track->tile.x + 1)*16) {
                UpdatePoint(x, y);
                depot_state = 0;
            }
            break;
        case DEPOT_TOP:
            y++;
            if (y > (track->tile.y + track_asset->bitmap_occupancy_y - 2)*16) {
                UpdatePoint(x, y);
                depot_state = 0;
            }
            break;
        case DEPOT_BOTTOM:
            y--;
            if (y < (track->tile.y + 1)*16) {
                UpdatePoint(x, y);
                depot_state = 0;
            }
            break;
    }
    return depot_state == 0;
}

void TrainBogie::Despawn() {
    if (track && GAME_STATE != 10) {
        track->num_trains--;
        track = nullptr;
    }
}

bool TrainBogie::MoveFront(Train* train) {
    Track* old_track = track;

    if (!track || !track->ok) {
        return false;
    }

    auto track_asset = dynamic_cast<TrackAsset*>(track->asset);

    if (tunnel_state == 4) {
        switch (track_asset->track_type) {
        case TUNNEL_LEFT:
            x++;
            break;
        case TUNNEL_RIGHT:
            x--;
            break;
        case TUNNEL_TOP:
            y++;
            break;
        case TUNNEL_BOTTOM:
            y--;
            break;
        default:
            break;
        }

        switch (track_asset->track_type) {
        case TUNNEL_LEFT:
            if (x >= 0) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_RIGHT:
            if (x < WORLD.screen_width) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_TOP:
            if (y >= 0) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_BOTTOM:
            if (y <= WORLD.screen_height) {
                tunnel_state = 0;
            }
            break;
        default:
            break;
        }

        return true;
    }

    if (tunnel_state == 2 || depot_state == 4 || depot_state == 5) {
        if (depot_state != 4 && depot_state != 5) {
            return false;
        }

        if (!ExitDepot()) {
            return true;
        }

        track->SetSomething(0);

        return true;
    }

    bool somevar = false; //bvar4

    if (track_asset->IsNextTrack(point)) {
        auto next_track = UpdateTrack();
        if (next_track == nullptr) {
            somevar = true;
            track_asset = dynamic_cast<TrackAsset*>(track->asset);
            if (track_asset->IsTunnel()) {
                auto track2 = dynamic_cast<Tunnel*>(train->forward_bogie->track);
                if (track2->field_11c == 1 && train->tunnel_state != 1) {
                    if(train->CanReverse()) {
                        train->SetDirection(direction == 0, true);
                        return false;
                    }

                    train->SetState(TrainState::WAITING);
                    train->field_90 = 1;
                    train->station_timer = 2;
                    return false;
                }

                track2->field_11c = 1;
                tunnel_state = 1;
                train->tunnel_state = 1;
                train->track_tile = train->forward_bogie->track->tile;
            } else if (track_asset->IsDepot()) {
                if (depot_state != 0) {
                    if(train->CanReverse()) {
                        train->SetDirection(direction == 0, true);
                        return false;
                    }

                    train->SetState(TrainState::WAITING);
                    train->field_90 = 1;
                    train->station_timer = 2;
                    return false;
                }

                if (false /* TODO */) {
                    depot_state = 1;
                }
            }

            if (old_track->field_118 == train) {
                old_track->field_118 = nullptr;
            }
        } else {
            if (next_track->track_type == TrackType::BUFFER) {
                train->SetState(TrainState::STOPPED);
                return false;
            } else if (next_track->track_type == TrackType::POINTS) {
                if (!UpdateTrackConfig(train, next_track)) {
                    return false;
                }
            } else if (next_track->track_type == TrackType::CROSSING) {
                int32_t num_minifigs = 0; // TODO
                bool move = false;

                if (num_minifigs == 0) {
                    next_track->SetSomething(0);
                    if (UpdateTrack() == nullptr) {
                        train->SetState(TrainState::DRIVE);
                        move = true;
                    } else {
                        train->SetState(TrainState::WAITING);
                    }
                } else {
                    train->SetState(TrainState::WAITING);
                    track = next_track; // ???
                }

                if (!move) {
                    return false;
                }
            } else if (next_track->track_type == TrackType::TUNNEL) {
                if (train->CanReverse()) {
                    train->SetDirection(direction == 0, true);
                    return false;
                }

                train->SetState(TrainState::WAITING);
                train->field_90 = 1;
                train->station_timer = 2;
                return false;
            } else {
                train->SetState(TrainState::WAITING);
                return false;
            }
        }
    }

    if (CheckStation(train)) {
        return false;
    }

    if (train->train_state == TrainState::WAITING) {
        if (track == nullptr) {
            train->SetState(TrainState::WAITING);
            return false;
        }

        if (track->track_type == TrackType::SWITCH) {
            train->UpdateSwitchState();
            return false;
        } else if (track->track_type == TrackType::POINTS) {
            if (track->num_trains > 1) {
                return false;
            }

            TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);

            if ((direction == 1 && point == 1) ||
                (direction == 0 && point == track_asset->num_points - 1) ||
                (direction == 1 && point == track_asset->num_points + 2) ||
                (direction == 0 && point == track_asset->num_points_alt - 1)) {
                train->SetState(TrainState::DRIVE);
                return false;
            }

            if (track->track_state == TrackState::CONFIG_A) {
                track->track_state = TrackState::CONFIG_B;
            } else if (track->track_state == TrackState::CONFIG_B) {
                track->track_state = TrackState::CONFIG_A;
            }

            return false;
        } else if (track->track_type == TrackType::CROSSING) {
            // TODO
        } else if (track->track_type == TrackType::TUNNEL) {
            if (train->CanReverse()) {
                train->SetDirection(train->direction == 0, true);
                return false;
            }

            train->SetState(TrainState::WAITING);
            train->field_90 = true;
            train->station_timer = 2;
            return false;
        } else {
            train->SetState(TrainState::WAITING);
            return false;
        }

        return false; // ???
    }

    if (track == nullptr) {
        return false;
    }

    bool moved = false;

    if (tunnel_state == 0 && depot_state == 0) {
        if (direction == 1) {
            point++;
        } else {
            point--;
        }

        moved = true;

        x = track_asset->points[point*2] + track->tile.x*16;
        y = track_asset->points[point*2+1] + track->tile.y*16;
    } else if (tunnel_state == 1 || depot_state == 1) {
        switch (track_asset->track_type) {
        case TrackAssetType::TUNNEL_LEFT:
        case TrackAssetType::DEPOT_LEFT:
            x--;
            break;
        case TrackAssetType::TUNNEL_RIGHT:
        case TrackAssetType::DEPOT_RIGHT:
            x++;
            break;
        case TrackAssetType::TUNNEL_TOP:
        case TrackAssetType::DEPOT_TOP:
            y--;
            break;
        case TrackAssetType::TUNNEL_BOTTOM:
        case TrackAssetType::DEPOT_BOTTOM:
            y++;
            break;
        default:
            break;
        }

        moved = true;

        if (tunnel_state == 1) {
            switch (track_asset->track_type) {
            case TrackAssetType::TUNNEL_LEFT:
                if (x <= 0) {
                    tunnel_state = 2;
                }
                break;
            case TrackAssetType::TUNNEL_RIGHT:
                if (x > WORLD.screen_width) {
                    tunnel_state = 2;
                }
                break;
            case TrackAssetType::TUNNEL_TOP:
                if (y <= 0) {
                    tunnel_state = 2;
                }
                break;
            case TrackAssetType::TUNNEL_BOTTOM:
                if (y >= WORLD.screen_height) {
                    tunnel_state = 2;
                }
                break;
            default:
                break;
            }
        }
    } else if (tunnel_state == 4) {
        switch (track_asset->track_type) {
        case TrackAssetType::TUNNEL_LEFT:
            x++;
            break;
        case TrackAssetType::TUNNEL_RIGHT:
            x--;
            break;
        case TrackAssetType::TUNNEL_TOP:
            y++;
            break;
        case TrackAssetType::TUNNEL_BOTTOM:
            y--;
            break;
        default:
            break;
        }

        switch (track_asset->track_type) {
        case TrackAssetType::TUNNEL_LEFT:
            if (x >= 0) {
                tunnel_state = 0;
            }
            break;
        case TrackAssetType::TUNNEL_RIGHT:
            if (x < WORLD.screen_width) {
                tunnel_state = 0;
            }
            break;
        case TrackAssetType::TUNNEL_TOP:
            if (y >= 0) {
                tunnel_state = 0;
            }
            break;
        case TrackAssetType::TUNNEL_BOTTOM:
            if (y <= WORLD.screen_height) {
                tunnel_state = 0;
            }
            break;
        default:
            break;
        }

        moved = true;
    } else if (depot_state == 4 || depot_state == 5) {
        if (ExitDepot()) {
            track->SetSomething(0);
        }

        moved = true;
    }

    if (track->track_type == TrackType::DEPOT) {
        if (train->depot_state == 4 || train->depot_state == 5) {
            if (track->num_trains != 0) {
                track->SetSomething(0);
                track->num_trains = 0;
                // TODO return
            }
        }
    }

    return moved;
}

void TrainBogie::ReverseDirection(Train* train) {
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);
    if (track_asset->IsNextTrack(point)) {
        if (UpdateTrack()) {
            direction = direction == 0;
        } else {
            direction = direction == 0;
        }
    } else {
        direction = direction == 0;
    }

    switch (tunnel_state) {
    case 0:
        if (train->tunnel_state == 1 && track->track_type == TrackType::TUNNEL) {
            tunnel_state = 1;
        }
        break;
    case 1:
    case 2:
        tunnel_state = 4;
        break;
    case 4:
    case 5:
        tunnel_state = 1;
        break;
    }

    if (tunnel_state == 4) {
        switch(track_asset->track_type) {
        case TUNNEL_LEFT:
            if (x > -1) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_RIGHT:
            if (x < WORLD.screen_width) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_TOP:
            if (y > -1) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_BOTTOM:
            if (y <= WORLD.screen_height) {
                tunnel_state = 0;
            }
            break;
        default:
            break;
        }

        if (tunnel_state == 0) {
            if (track->track_type == TrackType::TUNNEL) {
                UpdatePoint(x, y);
            }
        } else {
            switch (track_asset->track_type) {
            case TUNNEL_LEFT:
                point = track_asset->num_points - 1;
                break;
            case TUNNEL_RIGHT:
                point = 1;
                break;
            case TUNNEL_TOP:
                point = 1;
                break;
            case TUNNEL_BOTTOM:
                point = track_asset->num_points - 1;
                break;
            default:
                break;
            }
        }
    }

    switch (depot_state) {
    case 0:
        if (track->track_type != TrackType::DEPOT) {
            return;
        }

        if (train->depot_state != 4 && train->depot_state != 5) {
            if (train->depot_state != 0) {
                return;
            }
            int res_id = track->asset ? track->asset->id : -1;
            switch(res_id) {
            case 0xc54:
              if (this->direction == 0) {
                this->depot_state = 1;
                return;
              }
              return;
            case 0xc56:
            case 0xc5a:
              if (this->direction == 1) {
                this->depot_state = 1;
                return;
              }
              return;
            case 0xc58:
              if (this->direction != 0) {
                return;
              }
            default:
                return;
            }
        }

        this->depot_state = 1;
        break;
    case 1:
      this->depot_state = 4;
      break;
    case 4:
    case 5:
        this->depot_state = 1;
        break;
    default:
        break;
    }
}

// TODO needs checking
bool TrainBogie::Move(Train* train, TrainCar* train_segment) {
    if (!track) {
        return false;
    }

    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);

    if (tunnel_state == 2) {
        switch (track_asset->track_type) {
        case TUNNEL_LEFT:
        case DEPOT_LEFT:
            x--;
            break;
        case TUNNEL_RIGHT:
        case DEPOT_RIGHT:
            x++;
            break;
        case TUNNEL_TOP:
        case DEPOT_TOP:
            y--;
            break;
        case TUNNEL_BOTTOM:
        case DEPOT_BOTTOM:
            y++;
            break;
        default:
            break;
        }

        return true;
    }

    if (tunnel_state == 4) {
        switch (track_asset->track_type) {
        case TUNNEL_LEFT:
            x++;
            break;
        case TUNNEL_RIGHT:
            x--;
            break;
        case TUNNEL_TOP:
            y++;
            break;
        case TUNNEL_BOTTOM:
            y--;
            break;
        default:
            break;
        }

        switch (track_asset->track_type) {
        case TUNNEL_LEFT:
            if (x >= 0) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_RIGHT:
            if (x < WORLD.screen_width) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_TOP:
            if (y >= 0) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_BOTTOM:
            if (y <= WORLD.screen_height) {
                tunnel_state = 0;
            }
            break;
        default:
            break;
        }

        return true;
    }

    if (depot_state == 4 || depot_state == 5) {
        ExitDepot();
        return true;
    }

    if (track_asset->IsNextTrack(point)) {
        Track* crossing = train->GetCrossing();
        if (!UpdateTrack()) {
            track_asset = dynamic_cast<TrackAsset*>(track->asset);

            if (train->tunnel_state == 4 || !track_asset->IsTunnel()) {
                if (train->depot_state != 4 && train->depot_state != 5 && !track_asset->IsDepot()) {
                    depot_state = 1;
                    train_segment->depot_state = 1;
                }
            } else {
                tunnel_state = 1;
                train_segment->depot_state = 1;
            }

            if (crossing != nullptr && crossing->num_trains == 0) {
                crossing->SetSomething(1);
            }
        } else {
            switch (dynamic_cast<TrackAsset*>(track->asset)->track_type) {
            case TUNNEL_LEFT:
                if (x < 1) {
                    tunnel_state = 2;
                }
                break;
            case TUNNEL_RIGHT:
                if (x > WORLD.screen_width) {
                    tunnel_state = 2;
                }
                break;
            case TUNNEL_TOP:
                if (y < 1) {
                    tunnel_state = 2;
                }
                break;
            case TUNNEL_BOTTOM:
                if (y > WORLD.screen_height) {
                    tunnel_state = 2;
                }
                break;
            default:
                break;
            }

            if (tunnel_state != 2 && tunnel_state != 3) {
                train->SetState(TrainState::WAITING);
            } else {
                switch (dynamic_cast<TrackAsset*>(track->asset)->track_type) {
                case TUNNEL_LEFT:
                case DEPOT_LEFT:
                    x--;
                    break;
                case TUNNEL_RIGHT:
                case DEPOT_RIGHT:
                    x++;
                    break;
                case TUNNEL_TOP:
                case DEPOT_TOP:
                    y--;
                    break;
                case TUNNEL_BOTTOM:
                case DEPOT_BOTTOM:
                    y++;
                    break;
                default:
                    break;
                }
            }
        }
    }

    if (train->train_state == TrainState::DRIVE && track != nullptr && tunnel_state == 0 && depot_state == 0) {
        if (direction == 1) {
            point++;
        } else {
            point--;
        }
        x = track_asset->points[point*2] + track->tile.x*16;
        y = track_asset->points[point*2+1] + track->tile.y*16;
        return true;
    }

    if (tunnel_state != 1 && depot_state != 1) {
        if (tunnel_state != 4) {
            if (depot_state != 4) {
                if (depot_state != 5) {
                    return true;
                }
                ExitDepot();
                return true;
            }
            ExitDepot();
            return true;
        }

        switch (dynamic_cast<TrackAsset*>(track->asset)->track_type) {
        case TUNNEL_LEFT:
            x++;
            break;
        case TUNNEL_RIGHT:
            x--;
            break;
        case TUNNEL_TOP:
            y++;
            break;
        case TUNNEL_BOTTOM:
            y--;
            break;
        default:
            break;
        }

        switch (dynamic_cast<TrackAsset*>(track->asset)->track_type) {
        case TUNNEL_LEFT:
            if (x >= 0) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_RIGHT:
            if (x < WORLD.screen_width) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_TOP:
            if (y >= 0) {
                tunnel_state = 0;
            }
            break;
        case TUNNEL_BOTTOM:
            if (y <= WORLD.screen_height) {
                tunnel_state = 0;
            }
            break;
        default:
            break;
        }
        return true;
    }

    switch (dynamic_cast<TrackAsset*>(track->asset)->track_type) {
    case TUNNEL_LEFT:
    case DEPOT_LEFT:
        x--;
        break;
    case TUNNEL_RIGHT:
    case DEPOT_RIGHT:
        x++;
        break;
    case TUNNEL_TOP:
    case DEPOT_TOP:
        y--;
        break;
    case TUNNEL_BOTTOM:
    case DEPOT_BOTTOM:
        y++;
        break;
    default:
        break;
    }

    switch (dynamic_cast<TrackAsset*>(track->asset)->track_type) {
    case TUNNEL_LEFT:
        if (x <= 0) {
            tunnel_state = 2;
        }
        break;
    case TUNNEL_RIGHT:
        if (x > WORLD.screen_width) {
            tunnel_state = 2;
        }
        break;
    case TUNNEL_TOP:
        if (y <= 0) {
            tunnel_state = 2;
        }
        break;
    case TUNNEL_BOTTOM:
        if (y >= WORLD.screen_height) {
            tunnel_state = 2;
        }
        break;
    default:
        break;
    }
    return true;
}

void TrainBogie::Copy(TrainBogie* other) {
    direction = other->direction;
    point = other->point;
    x = other->x;
    y = other->y;
    track = other->track;
    tunnel_state = other->tunnel_state;
    depot_state = other->depot_state;
}
