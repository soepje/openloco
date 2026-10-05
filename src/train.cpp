#include <cstdint>

#include "train.h"
#include "asset.h"
#include "building.h"
#include "track.h"
#include "train_bogie.h"
#include "train_car.h"

Train::Train(uint32_t engine_resource_id, uint32_t unk1, bool unk2, bool unk3) {
    track_tile = {-1, -1};
    field_32 = {-1, -1};

    field_88 = unk3;
    field_4 = unk1;

    reversing = false;
    field_90 = false;
    on_bridge = false;

    field_8c = 0;
    field_68 = 0;
    field_70 = 0;

    cars[0] = nullptr;
    cars[1] = nullptr;
    cars[2] = nullptr;
    cars[3] = nullptr;

    // TODO not to sure about this
    for (size_t i = 0; i < 8; i++) {
        field_38[i] = 0;
    }

    forward_bogie = new TrainBogie(unk2);

    crash_timer = 0;
    last_car = 0;

    cars[0] = new TrainCar(engine_resource_id, TrainCarType::ENGINE, unk2);

    TrainCar* segment = cars[last_car];
    if (segment) {
        if (segment->ok) {
            segment->train = this;

            TrainAsset* train_asset = static_cast<TrainAsset*>(segment->asset);
            speed_slow = train_asset->speed_slow;
            speed_fast = train_asset->speed_fast;
            speed = train_asset->speed_slow;

            // point = 0;

            SetState(TrainState::UNKNOWN_1);

            station_timer = 0;

            // TODO
        } else {
            delete segment;
            cars[last_car] = nullptr;
        }
    }
}

void Train::Update() {
    if (field_90) {
        field_90 = false;
        if (!reversing) {
            reversing = true;
            ReverseDirection();
            reversing = false;
        }
    }

    if (station_timer != 0) {
        station_timer--;
        if (station_timer == 1 && train_state == TrainState::STOPPED) {
            SetState(TrainState::DRIVE);
        }
    }

    int local_c = 0;

    if (train_state == TrainState::UNKNOWN_1 || train_state == TrainState::CRASHED || tunnel_state == 2 || tunnel_state == 3 || depot_state == 2 || speed == 0 || station_timer > 0 || (train_state != TrainState::DRIVE && train_state != TrainState::STOPPED)) {

    } else {
        bool go = false;
        Track* track = forward_bogie->track;
        if (track) {
            switch (track->track_state) {
            case TrackState::SWITCH_GO:
                SetState(TrainState::DRIVE);
                go = true;
                break;
            case TrackState::SWITCH_REVERSE:
                if (!reversing) {
                    reversing = true;
                    ReverseDirection();
                    reversing = false;
                }
                SetState(TrainState::DRIVE);
                go = true;
                break;
            case TrackState::SWITCH_STOP:
                if (train_state != TrainState::STOPPED) {
                    SetState(TrainState::STOPPED);
                }
                break;
            default:
                go = true;
                break;
            }
        }

        if (go) {
            for (size_t i = 0; i < speed; i++) {

                // what is this about???
                if (forward_bogie->tunnel_state == 2 || forward_bogie->depot_state == 2) {
                    local_c = 1;
                } else {
                    if (!forward_bogie->Unk2(this)) {
                        break;
                    }
                    local_c++;
                    if (forward_bogie->tunnel_state == 2 || forward_bogie->tunnel_state == 3) {
                        local_c = 1;
                    } else if (depot_state != 2 && forward_bogie->depot_state == 2) {
                        depot_state = 1;
                        SetVisible(true);
                        local_c = 1;
                    }
                }



            }

            // TOOD
        }
    }
}

bool Train::HasPassengerCar() {
    for (size_t i = 1; i < 4; i++) {
        if (cars[i] != nullptr && cars[i]->train_car_type == TrainCarType::PASSENGER) {
            return true;
        }
    }
    return false;
}

void Train::SetVisible(bool param) {
    // TODO
}

void Train::SetState(TrainState state) {
    if (train_state == state) {
        return;
    }

    if (field_90) {
        return;
    }

    train_state = state;

    if (state == TrainState::UNKNOWN_1) {
        crash_timer = 0;
        station_timer = 0;
    } else if (state != TrainState::STOPPED) {

    } else if (state != TrainState::CRASHED) {
        if (field_68 != 0) {
            return;
        }
        if (tunnel_state == 2 || tunnel_state == 3) {
            return;
        }

        // TODO
    } else {
        station_timer = 0;
    }

    for (size_t i = 0; i < last_car + 1; i++) {
        // TODO
    }
}

bool Train::IsPlainTrack() {
    if (train_state == TrainState::CRASHED || reversing) {
        return false;
    }

    TrainBogie* bogey = nullptr;
    if (direction == 0) {
        bogey = cars[last_car]->bogie_back;
    } else {
        bogey = cars[0]->bogie_front;
    }

    if (forward_bogie->track) {
        auto track_asset = dynamic_cast<TrackAsset*>(forward_bogie->track->asset);
        if (!track_asset->IsTunnel() && !track_asset->IsDepot()) {
            return true;
        }

        if (bogey->track) {
            auto track_asset2 = dynamic_cast<TrackAsset*>(bogey->track->asset);
            if (!track_asset2->IsTunnel() && !track_asset2->IsDepot()) {
                return true;
            }

            // ???
            if (track_asset != track_asset2) {
                return false;
            }
        }
    }

    return true;
}

Track* Train::GetTrack() {
    TrainBogie* bogie = nullptr;
    if (direction == 0) {
        bogie = cars[last_car]->bogie_back;
    } else {
        bogie = cars[0]->bogie_front;
    }
    return nullptr; // TODO
}

bool Train::IsOnBridge() {
    on_bridge = false;
    for (size_t i = 0; i <= last_car; i++) {
        if (cars[i]->on_bridge) {
            on_bridge = true;
            return on_bridge;
        }
    }
    return on_bridge;
}

bool Train::AddCar(int32_t resource_id, TrainCarType type, bool tunnel) {
    if (last_car < 3 && !cars[last_car+1]) {
        last_car++;
        TrainCar* train_car = new TrainCar(resource_id, type, tunnel);
        cars[last_car] = train_car;
        if (train_car) {
            if (train_car->ok) {
                train_car->train = this;
                return true;
            }
            delete train_car;
            cars[last_car] = nullptr;
        }
        last_car--;
    }
    return false;
}

void Train::ExitDepot(Depot* depot, bool unk) {
    track_tile = depot->tile;
    if (depot->train && depot->train != this) {
        depot->QueueTrain(this);
        return;
    }
    depot->field_128 = true;
    depot->train = this;
    depot_state = 5;
    SetVisible(true);
    for (size_t i = 0; i <= last_car; i++) {
        cars[last_car]->depot_state = 5;
        cars[last_car]->bogie_front->depot_state = 5;
        cars[last_car]->bogie_back->depot_state = 5;
    }
    SetState(TrainState::DRIVE);
    SetVisible(true);
    SetTrack(depot, unk);
    depot->field_11c = 0;
}

// More like set spawn track, this code assumes track is a depot or tunnel
bool Train::SetTrack(Track* track, bool unk) {
    if (!track) {
        return false;
    }

    int track_type = 0;
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);

    if (track_asset->IsTunnel()) {
        track_type = 2;
        track_tile = track->tile;
    } else if (track_asset->IsDepot()) {
        track_type = 1;
        track->SetSomething(1);
    }

    for (size_t i = 0; i <= last_car; i++) {
        TrainCar* car = cars[i];
        TrainBogie* bogie_front = car->bogie_front;
        TrainBogie* bogie_back = car->bogie_back;

        bogie_front->track = track;
        bogie_back->track = track;

        if (track_type == 2) {
          car->tunnel_state = 4;
          bogie_front->tunnel_state = 4;
          bogie_back->tunnel_state = 4;
        } else {
          bogie_front->depot_state = 5;
          bogie_back->depot_state = 5;
        }

        if (track_asset->track_type == TUNNEL_LEFT || track_asset->track_type == DEPOT_LEFT) {
            if (!unk) {
                car->rotation = direction == 0 ? 64 : 0;
            }

            bogie_front->direction = 0;
            bogie_front->point = track_asset->num_points - 1;
            bogie_back->direction = 0;
            bogie_back->point = track_asset->num_points - 1;
        } else if (track_asset->track_type == TUNNEL_RIGHT || track_asset->track_type == DEPOT_RIGHT) {
            if (!unk) {
                car->rotation = direction == 0 ? 0 : 64;
            }

            bogie_front->direction = 1;
            bogie_front->point = 1;
            bogie_back->direction = 1;
            bogie_back->point = 1;
        } else if (track_asset->track_type == TUNNEL_TOP || track_asset->track_type == DEPOT_TOP) {
            if (!unk) {
                car->rotation = direction == 0 ? 32 : 96;
            }

            bogie_front->direction = 1;
            bogie_front->point = 1;
            bogie_back->direction = 1;
            bogie_back->point = 1;
        } else if (track_asset->track_type == TUNNEL_BOTTOM || track_asset->track_type == DEPOT_BOTTOM) {
            if (!unk) {
                car->rotation = direction == 0 ? 96 : 32;
            }

            bogie_front->direction = 0;
            bogie_front->point = track_asset->num_points - 1;
            bogie_back->direction = 0;
            bogie_back->point = track_asset->num_points - 1;
        }
    }

    forward_bogie->track = track;
    forward_bogie->direction = cars[0]->bogie_front->direction;
    forward_bogie->point = cars[0]->bogie_front->point;
    forward_bogie->x = track_asset->points[forward_bogie->point] + track->tile.x;
    forward_bogie->y = track_asset->points[forward_bogie->point+1] + track->tile.y;

    if (track_type == 2) {
        forward_bogie->tunnel_state = 4;
    } else {
        forward_bogie->depot_state = 5;
    }

    if (direction != 0) {
        // TODO
    }

    // TODO

    return false;
}
