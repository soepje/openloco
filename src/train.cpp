#include <cstdint>

#include "train.h"
#include "asset.h"
#include "building.h"
#include "track.h"
#include "train_bogie.h"

Train::Train(uint32_t engine_resource_id, uint32_t unk1, bool unk2, bool unk3) {
    field_2e = -1;
    field_30 = -1;
    field_32 = -1;
    field_34 = -1;

    field_88 = unk3;
    field_4 = unk1;

    field_5a = false;
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

    cars[0] = new TrainCar(engine_resource_id, 2, unk2);

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
        if (!field_5a) {
            field_5a = true;
            ReverseDirection();
            field_5a = false;
        }
    }

    if (station_timer != 0) {
        station_timer--;
        if (station_timer == 1 && train_state == TrainState::STOPPED) {
            SetState(TrainState::DRIVE);
        }
    }

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
                if (!field_5a) {
                    field_5a = true;
                    ReverseDirection();
                    field_5a = false;
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


                // this_00 = this->train_thingy;
                // if ((this_00->tunnel_state == 2) || (this_00->depot_state == 2)) {
                //   local_c = 1;
                // }
                // else {
                //   bVar5 = TrainBogie_Unk2(this_00,this);
                //   if (!bVar5) break;
                //   local_c = local_c + 1;
                //   iVar10 = this->train_thingy->tunnel_state;
                //   if ((iVar10 == 2) || (iVar10 == 3)) {
                //     local_c = 1;
                //   } else if ((this->depot_state != 2) && (this->train_thingy->depot_state == 2)) {
                //     this->depot_state = 1;
                //     Train_SetVisible(this,true);
                //     local_c = 1;
                //   }
                // }

            }

            // TOOD
        }
    }
}

bool Train::HasPassengerCar() {
    for (size_t i = 1; i < 4; i++) {
        if (cars[i] != nullptr && cars[i]->train_type == 2) {
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
    if (train_state == TrainState::CRASHED || field_5a) {
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
