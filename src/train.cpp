#include <cstdint>

#include "train.h"
#include "asset.h"
#include "building.h"
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

    field_20 = new TrainBogie(unk2);

    field_28 = 0;
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

            Unk2(0);

            field_36 = 0;

            // TODO
        } else {
            delete segment;
            cars[last_car] = nullptr;
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

void Train::Unk2(int32_t param_1) {
    if (field_5c == param_1) {
        return;
    }

    if (field_90) {
        return;
    }

    field_5c = param_1;

    if (param_1 == 0) {
        field_28 = 0;
        field_36 = 0;
    } else if (param_1 != 1) {

    } else if (param_1 != 4) {
        if (field_68 != 0) {
            return;
        }
        if (tunnel_state == 2 || tunnel_state == 3) {
            return;
        }

        // TODO
    } else {
        field_36 = 0;
    }

    for (size_t i = 0; i < last_car + 1; i++) {
        // TODO
    }
}

bool Train::IsPlainTrack() {
    if (field_5c == 4 || field_5a) {
        return false;
    }

    TrainBogie* bogey = nullptr;
    if (direction == 0) {
        bogey = cars[last_car]->bogie_back;
    } else {
        bogey = cars[0]->bogie_front;
    }

    if (field_20->track) {
        auto track_asset = dynamic_cast<TrackAsset*>(field_20->track->asset);
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
