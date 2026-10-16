#include <cstdint>
#include <endian.h>

#include "train.h"
#include "asset.h"
#include "building.h"
#include "rect.h"
#include "track.h"
#include "train_bogie.h"
#include "train_car.h"
#include "globals.h"
#include "tunnel.h"

Train::Train(uint32_t engine_resource_id, uint32_t unk1, bool tunnel, bool unk3) {
    track_tile = {-1, -1};
    depot_tile = {-1, -1};

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

    for (size_t i = 0; i < 8; i++) {
        passengers[i] = nullptr;
    }

    forward_bogie = new TrainBogie(tunnel);

    crash_timer = 0;
    last_car = 0;

    cars[0] = new TrainCar(engine_resource_id, TrainCarType::ENGINE, tunnel);

    TrainCar* segment = cars[last_car];
    if (segment) {
        if (segment->ok) {
            segment->train = this;

            TrainAsset* train_asset = static_cast<TrainAsset*>(segment->asset);
            speed_slow = train_asset->speed_slow;
            speed_fast = train_asset->speed_fast;
            speed = train_asset->speed_slow;
            direction = 0;

            SetState(TrainState::UNKNOWN_1);

            station_timer = 0;

            if (false) {
                // TODO
            } else {
                field_78 = 1;
                field_7c = 1;
            }

            if (!tunnel) {
                // TODO increment counter
                field_7a = 0;
                forward_bogie->SetTrack(-1, -1);
                // TODO
            }

            field_89 = false;
            field_8a = false;
            field_70 = 0;

            if (!tunnel) {
                depot_state = 2;
                tunnel_state = 0;
                SetVisible(false);
            } else {
                tunnel_state = 2;
                depot_state = 0;
                SetVisible(true);
            }
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

    if (!(train_state == TrainState::UNKNOWN_1 || train_state == TrainState::CRASHED ||
        tunnel_state == 2 || tunnel_state == 3 || depot_state == 2 || speed == 0 || station_timer > 0 ||
        (train_state != TrainState::DRIVE && train_state != TrainState::STOPPED))) {

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

                if (train_state == TrainState::DRIVE) {
                    TrainCar* engine = cars[0];
                    int resource_id = engine->asset == nullptr ? -1 : engine->asset->id;
                    if (resource_id == 6152 && engine->visible && engine->depot_state == 0 && engine->tunnel_state == 0) {
                        if (puff_timer == 10) {
                            // TODO trigger effect
                            puff_timer = 0;
                        } else {
                            puff_timer++;
                        }
                    }
                    if (direction == 0) {
                        for (size_t j = 0; j <= last_car; j++) {
                            cars[j]->Update(this);
                        }
                    } else if (direction == 1) {
                        for (size_t j = last_car; j >= 0; j--) {
                            cars[j]->Update(this);
                        }
                    }
                }

                Track* forward_bogie_track = forward_bogie->track;
                if (forward_bogie_track != nullptr && forward_bogie_track->track_type == TrackType::SWITCH && train_state != TrainState::UNKNOWN_1) {
                    if (forward_bogie_track->track_state == TrackState::SWITCH_GO) {
                        SetState(TrainState::DRIVE);
                    } else if (forward_bogie_track->track_state == TrackState::SWITCH_REVERSE) {
                        if (reversing) {
                            SetState(TrainState::DRIVE);
                        } else {
                            reversing = true;
                            ReverseDirection();
                            reversing = false;
                            SetState(TrainState::DRIVE);
                        }
                    } else if (forward_bogie_track->track_state == TrackState::SWITCH_STOP) {
                        if (train_state != TrainState::STOPPED) {
                            SetState(TrainState::STOPPED);
                        }
                    }
                }

                if (tunnel_state == 1) {
                    if (cars[last_car]->tunnel_state == 2 && cars[0]->tunnel_state == 2) {
                        tunnel_state = 2;
                        forward_bogie->tunnel_state = 2;

                        Tunnel* tunnel = dynamic_cast<Tunnel*>(forward_bogie->track);
                        tunnel->field_11c = 0;
                        switch (tunnel->asset->id) {
                        case 3138:
                        case 3140:
                        case 3142:
                        case 3144:
                            tunnel_state = 3;
                            SetVisible(false);
                            field_68 = 1;
                            break;
                        default:
                            Tunnel* out_tunnel = dynamic_cast<Tunnel*>(BUILDING_MANAGER.GetRandomBuilding(0));
                            if (out_tunnel == nullptr) {
                                SetState(TrainState::UNKNOWN_2);
                            } else {
                                out_tunnel->QueueTrain(this);
                            }
                            break;
                        }
                    }
                } else if (depot_state == 1) {
                    if (cars[last_car]->depot_state == 2 && cars[0]->depot_state == 2) {
                        depot_state = 2;
                        SetVisible(false);
                        forward_bogie->depot_state = 2;
                        Depot* building = dynamic_cast<Depot*>(WORLD.GetBuilding(forward_bogie->track->tile.x, forward_bogie->track->tile.y + 1, 0));
                        if (building == nullptr) {
                            return;
                        }
                        building->train = this;
                        building->SetSomething(1);
                        building->field_11c = 1;
                        depot_tile = building->tile;
                    }
                }

                if (depot_state == 4 || depot_state == 5) {
                    // TODO call something in train
                }
            }

            if (field_68 == 0 && tunnel_state != 2 && tunnel_state != 3) {
                for (size_t j = 0; j <= last_car; j++) {
                    TrainCar* car = cars[j];
                    if (car->field_48 != nullptr) {
                        // TODO call something
                    }
                }
            }
        }
    }

    if (tunnel_state == 2 || (tunnel_state == 3 && field_68 != 1)) {
        Building* b = WORLD.GetBuilding(track_tile.x, track_tile.y + 1, 0);
        if (b == nullptr) {
            Tunnel* t = dynamic_cast<Tunnel*>(BUILDING_MANAGER.GetRandomBuilding(4));
            if (t == nullptr) {
                SetState(TrainState::UNKNOWN_2);
                return;
            }
            t->QueueTrain(this);
        }
    }

    if (train_state == TrainState::CRASHED) {
        if (crash_timer == 0) {
            SetState(TrainState::UNKNOWN_1);
            crash_timer = 0;
            SetSpeed(speed_slow);
            if (!reversing && CanReverse()) {
                reversing = true;
                ReverseDirection();
                reversing = false;
            }
            SetState(TrainState::DRIVE);
        } else {
            crash_timer--;
        }
    }

    if (local_c != 0 && depot_state == 0 && 3 < depot_state && depot_state < 6) {
        int16_t x = forward_bogie->x < 0 ? -1 : forward_bogie->x / 16;
        int16_t y = forward_bogie->y < 0 ? -1 : forward_bogie->y / 16;
        // TODO call something in train mystery
    }

    if (forward_bogie->track != nullptr && depot_state != 2 && tunnel_state != 2 && tunnel_state != 3) {
        int32_t resource_id = forward_bogie->track->asset->id;
        if (resource_id == 3164 || resource_id == 31646 || resource_id == 3168) {
            // TODO call something in train mystery
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

void Train::SetVisible(bool visible) {
    if (tunnel_state != 2 && tunnel_state != 3 && field_68 == 0) {
        if (cars[0]->field_48 == nullptr) {
            cars[0]->TriggerSound();
        }

        for (size_t i = 0; i <= last_car; i++) {
            TrainCar* car = cars[i];
            if (car != nullptr) {
                if (car->visible == visible) {
                    continue;
                }

                car->visible = visible;

                WORLD.MarkDirty(car->view);

                if (car->field_48 != nullptr) {
                    if (visible && train_state != TrainState::UNKNOWN_1 && train_state != TrainState::STOPPED && train_state != TrainState::CRASHED) {
                        // TODO play sound
                    } else {
                        // TODO stop sound
                    }
                }
            }
        }
    }
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

bool Train::CanReverse() {
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

Track* Train::GetCrossing() {
    TrainBogie* bogie = nullptr;
    if (direction == 0) {
        bogie = cars[last_car]->bogie_back;
    } else {
        bogie = cars[0]->bogie_front;
    }

    if (bogie->track->track_type == TrackType::CROSSING) {
        return bogie->track;
    }

    return nullptr;
}

bool Train::IsOnBridge() {
    on_bridge = false;
    for (size_t i = 0; i <= last_car; i++) {
        if (cars[i]->z) {
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

void Train::ExitDepot(Depot* depot, bool keep_orientation) {
    depot_tile = depot->tile;
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
    SetStartingTrack(depot, keep_orientation);
    depot->field_11c = 0;
}

bool Train::SetStartingTrack(Track* track, bool keep_orientation) {
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
            if (!keep_orientation) {
                car->rotation = direction == 0 ? 64 : 0;
            }

            bogie_front->direction = 0;
            bogie_front->point = track_asset->num_points - 1;
            bogie_back->direction = 0;
            bogie_back->point = track_asset->num_points - 1;
        } else if (track_asset->track_type == TUNNEL_RIGHT || track_asset->track_type == DEPOT_RIGHT) {
            if (!keep_orientation) {
                car->rotation = direction == 0 ? 0 : 64;
            }

            bogie_front->direction = 1;
            bogie_front->point = 1;
            bogie_back->direction = 1;
            bogie_back->point = 1;
        } else if (track_asset->track_type == TUNNEL_TOP || track_asset->track_type == DEPOT_TOP) {
            if (!keep_orientation) {
                car->rotation = direction == 0 ? 32 : 96;
            }

            bogie_front->direction = 1;
            bogie_front->point = 1;
            bogie_back->direction = 1;
            bogie_back->point = 1;
        } else if (track_asset->track_type == TUNNEL_BOTTOM || track_asset->track_type == DEPOT_BOTTOM) {
            if (!keep_orientation) {
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
    forward_bogie->x = track_asset->points[forward_bogie->point] + track->tile.x * 16;
    forward_bogie->y = track_asset->points[forward_bogie->point+1] + track->tile.y * 16;

    if (track_type == 2) {
        forward_bogie->tunnel_state = 4;
    } else {
        forward_bogie->depot_state = 5;
    }

    if (direction == 0) {
        int32_t j = 0;

        for (size_t i = 0; i <= last_car; i++) {
            TrainCar* car = cars[i];
            switch (track_asset->track_type) {
            case TUNNEL_LEFT:
            case DEPOT_LEFT:
                car->bogie_front->x = i == 0 ?
                    forward_bogie->x - 12 : cars[0]->bogie_front->x - j;
                car->bogie_front->y = forward_bogie->y;
                car->bogie_back->x = car->bogie_front->x - 22;
                car->bogie_back->y = forward_bogie->y;
                break;
            case TUNNEL_RIGHT:
            case DEPOT_RIGHT:
                car->bogie_front->x = i == 0 ?
                    forward_bogie->x + 12 : cars[0]->bogie_front->x + j;
                car->bogie_front->y = forward_bogie->y;
                car->bogie_back->x = car->bogie_front->x + 22;
                car->bogie_back->y = forward_bogie->y;
                break;
            case TUNNEL_TOP:
            case DEPOT_TOP:
                car->bogie_front->y = i == 0 ?
                    forward_bogie->y - 12 : cars[0]->bogie_front->y - j;
                car->bogie_front->x = forward_bogie->x;
                car->bogie_back->x = forward_bogie->x;
                car->bogie_back->y = car->bogie_front->y - 22;
                break;
            case TUNNEL_BOTTOM:
            case DEPOT_BOTTOM:
                car->bogie_front->y = i == 0 ?
                    forward_bogie->y + 12 : cars[0]->bogie_front->y + j;
                car->bogie_front->x = forward_bogie->x;
                car->bogie_back->x = forward_bogie->x;
                car->bogie_back->y = car->bogie_front->y + 22;
                break;
            default:
                break;
            }

            j += 38;
        }
    } else if (direction == 1) {
        int32_t j = 0;

        for (size_t i = last_car; i >= 0; i--) {
            TrainCar* car = cars[i];
            switch (track_asset->track_type) {
            case TUNNEL_LEFT:
            case DEPOT_LEFT:
                car->bogie_back->x = i == last_car ?
                    forward_bogie->x - 12 : cars[last_car]->bogie_back->x - j;
                car->bogie_back->y = forward_bogie->y;
                car->bogie_front->x = car->bogie_back->x - 22;
                car->bogie_front->y = forward_bogie->y;
                break;
            case TUNNEL_RIGHT:
            case DEPOT_RIGHT:
                car->bogie_back->x = i == last_car ?
                    forward_bogie->x + 12 : cars[last_car]->bogie_back->x + j;
                car->bogie_back->y = forward_bogie->y;
                car->bogie_front->x = car->bogie_back->x + 22;
                car->bogie_front->y = forward_bogie->y;
                break;
            case TUNNEL_TOP:
            case DEPOT_TOP:
                car->bogie_back->y = i == last_car ?
                    forward_bogie->y - 12 : cars[last_car]->bogie_back->y - j;
                car->bogie_back->x = forward_bogie->x;
                car->bogie_front->x = forward_bogie->x;
                car->bogie_front->y = car->bogie_back->y - 22;
                break;
            case TUNNEL_BOTTOM:
            case DEPOT_BOTTOM:
                car->bogie_back->y = i == last_car ?
                    forward_bogie->y + 12 : cars[last_car]->bogie_back->y + j;
                car->bogie_back->x = forward_bogie->x;
                car->bogie_front->x = forward_bogie->x;
                car->bogie_front->y = car->bogie_back->y + 22;
                break;
            default:
                break;
            }

            j += 38;
        }
    }

    return true;
}

int16_t Train::SetSpeed(int16_t speed) {
    TrainCar* engine = cars[0];
    if (engine == nullptr) {
        return 0;
    }

    if (speed == speed_slow) {
        this->speed = speed;
        engine->SetSomething(0);
        engine->SetFrame(engine->current_frame, false);
    } else if (speed == speed_fast) {
        this->speed = speed;
        engine->SetSomething(1);
        engine->SetFrame(engine->current_frame, false);
    }

    return this->speed;
}

bool Train::ReverseDirection() {
    if (depot_state == 2 || tunnel_state == 2 || tunnel_state == 3) {
        return false;
    }

    if (tunnel_state == 1) {
        tunnel_state = 4;
    } else if (tunnel_state == 4) {
        tunnel_state = 1;
    }

    direction = direction == 0 ? 1 : 0;

    for (size_t i = 0; i <= last_car; i++) {
        TrainCar* car = cars[i];
        car->bogie_front->ReverseDirection(this);
        car->bogie_back->ReverseDirection(this);

        if (car->tunnel_state > 0) {
            if (car->tunnel_state < 3) {
                car->tunnel_state = 4;
            } else if (car->tunnel_state == 4) {
                car->tunnel_state = 1;
            }
        }

        if (car->bogie_front->tunnel_state == 1 || car->bogie_back->tunnel_state == 1) {
            car->tunnel_state = 1;
        }

        switch (car->depot_state) {
        case 1:
            car->depot_state = 4;
            break;
        case 2:
            car->depot_state = 5;
            car->visible = false;
            break;
        case 4:
            car->depot_state = 1;
            car->UpdateDepotEnterVisibility(this);
            break;
        case 5:
            if (car->bogie_front->depot_state == 1 || car->bogie_back->depot_state == 1) {
                car->depot_state = 1;
                car->UpdateDepotEnterVisibility(this);
            } else {
                car->depot_state = 2;
                car->visible = false;
            }
            break;
        default:
           break;
        }
    }

    forward_bogie->track->num_trains--;

    bool ok;

    if (direction == 1) {
        forward_bogie->Copy(cars[last_car]->bogie_back);
        forward_bogie->track->num_trains++;
        for (size_t i = 0; i < 12; i++) {
            ok = forward_bogie->Unk2(this);
        }
    } else {
        forward_bogie->Copy(cars[0]->bogie_front);
        forward_bogie->track->num_trains++;
        for (size_t i = 0; i < 12; i++) {
            ok = forward_bogie->Unk2(this);
            if(!ok) {
                break;
            }
        }
    }

    if ((tunnel_state == 4) && (forward_bogie->tunnel_state == 2)) {
      forward_bogie->tunnel_state = 4;
    }

    forward_bogie->CheckTunnel();
    forward_bogie->CheckDepot();

    switch (forward_bogie->depot_state) {
    case 0:
        if (cars[0]->depot_state != 4 && cars[0]->depot_state != 5 &&
            cars[last_car]->depot_state != 4 && cars[last_car]->depot_state != 5) {
            if (depot_state == 0) {
                break;
            }
            Depot* depot = dynamic_cast<Depot*>(WORLD.GetBuilding(depot_tile.x, depot_tile.y + 1, 0));
            if (depot != nullptr && depot->train == this) {
                depot->field_11c = 0;
                depot->field_128 = false;
                depot->SetSomething(0);
                depot->train = nullptr;
            }
            depot_tile = {-1, -1};
            depot_state = 0;
            SetVisible(true);
        } else {
            depot_state = 4;
            SetVisible(true);
        }
        break;
    case 1:
    case 2:
        depot_state = 1;
        SetVisible(true);
        break;
    case 4:
    case 5:
        depot_state = 4;
        SetVisible(true);
        break;
    }

    return ok;
}
