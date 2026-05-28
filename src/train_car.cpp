#include <cmath>
#include <cstdint>

#include "asset.h"

#include "train_car.h"
#include "globals.h"
#include "rect.h"
#include "track.h"
#include "train_bogie.h"
#include "train.h"

TrainCar::TrainCar(uint32_t resource_id_, uint32_t train_type_, bool tunnel) : ImageEntity(resource_id_, -1, 0, 0) {
    bogie_front = nullptr;
    bogie_back = nullptr;
    train = nullptr;
    resource_id = resource_id_;
    if (ok) {
        train_type = train_type_;
        bogie_front = new TrainBogie(tunnel);
        bogie_back = new TrainBogie(tunnel);
        rotation = 0;
        field_43a = 0;
        field_43c = 0;
        if (tunnel) {
            depot_state = 0;
            tunnel_state = 2;
        } else {
            depot_state = 2;
            tunnel_state = 0;
        }
        on_bridge = 0;
        SetRect(view, 0, 0, 0, 0);
        SetFrame(rotation, true);
        field_424 = 0;
        visible = false;
    }
}

static int GetCarType(uint32_t resource_id) {
    switch(resource_id) {
    case 0x1804:
    case 0x1806:
    case 0x1808:
      return 1; // engine
    case 0x1866:
    case 0x1868:
    case 0x186a:
      return 2; // passenger car
    case 0x186c:
    case 0x186e:
      return 3; // freight car
    case 0x1870:
    case 0x1871:
      return 4; // mail car
    default:
      return 0;
    }
}

bool TrainCar::SetAsset(uint32_t resource_id_, int32_t frame_set) {
    resource_id = resource_id_;
    if (ImageEntity::SetAsset(resource_id_, frame_set, false)) {
        train_type = GetCarType(resource_id);
        return true;
    }
    return false;
}

void TrainCar::Update(Train* train) {
    TrainBogie* forward_bogie  = (train->direction == 0) ? bogie_front : bogie_back;

    bool moved = false;

    if (bogie_front->track && bogie_back->track) {
        if (bogie_front->track->track_type == TrackType::BRIDGE && bogie_back->track->track_type == TrackType::BRIDGE && bogie_front->track == bogie_back->track) {
            train->on_bridge = true;
            TrackAsset* track_asset = dynamic_cast<TrackAsset*>(bogie_front->track->asset);
            if (track_asset->track_type == TrackAssetType::BRIDGE_HORIZONTAL) {
                moved = MoveBridge(train);
            } else if (bogie_front->track->view.bottom - 32 < bogie_front->y || bogie_back->track->view.bottom - 32 < bogie_back->y || bogie_front->track != bogie_back->track) {
                on_bridge = 0;
                moved = false;
            } else {
                on_bridge = 1;
                moved = false;
            }
        } else {
            on_bridge = 0;
            train->IsOnBridge();
        }
    }

    if (!moved) {
        if (bogie_front->Move(train, this) || bogie_back->Move(train, this)) {
            WORLD.MarkDirty(view);
            UpdateRotation();
            SetFrame(rotation, true);
            UpdatePosition();
        }

        if (train->tunnel_state != 0) {
            // TODO
        }

        if (train->depot_state != 0) {
            // TODO
        }
    }

    // TODO
}

void TrainCar::UpdatePosition() {
    TrainAsset* train_asset = dynamic_cast<TrainAsset*>(asset);

    view.left = bogie_front->x - train_asset->train_data[0][rotation][0];
    view.top = bogie_front->y - train_asset->train_data[0][rotation][1];
    view.right = view.left + asset->frame_width;
    view.bottom = view.top + asset->frame_height;

    SetViewPosition(view.left, view.top);
}

void TrainCar::UpdateRotation() {
    int32_t x1 = bogie_front->x;
    int32_t y1 = bogie_front->y;
    int32_t x2 = bogie_back->x;
    int32_t y2 = bogie_back->y;

    int32_t dx = x1 - x2;
    int32_t dy = y1 - y2;

    double degrees = std::atan2((double) dy, (double) dx) * (180.0 / 3.14159265359);
    int32_t frame = std::round(degrees * (128.0 / 360.0));

    rotation = (frame == 128) ? 0 : frame;
}

bool TrainCar::UpdateDepotEnterVisibility(Train* train) {
    TrainBogie* forward_bogie  = (train->direction == 0) ? bogie_front : bogie_back;
    Track* track = forward_bogie->track;
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);

    if (forward_bogie->depot_state == 1) {
        switch (track_asset->track_type) {
        case DEPOT_LEFT:
            if (view.right < (track->tile_x + track_asset->bitmap_occupancy_x - 1) * 16) {
                depot_state = 2;
            }
            break;
        case DEPOT_RIGHT:
            if (view.left > (track->tile_x + 1) * 16) {
                depot_state = 2;
            }
            break;
        case DEPOT_TOP:
            if (view.bottom < (track->tile_x + track_asset->bitmap_occupancy_y - 2) * 16) {
                depot_state = 2;
            }
            break;
        case DEPOT_BOTTOM:
            if (view.top > (track->tile_y + 1) * 16) {
                depot_state = 2;
            }
            break;
        default:
            break;
        }
    }

    if (depot_state == 2) {
        SetVisible(false);
    }

    return false;
}

bool TrainCar::UpdateDepotExitVisibility(Train* train) {
    TrainBogie* forward_bogie  = (train->direction == 0) ? bogie_front : bogie_back;
    Track* track = forward_bogie->track;
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(track->asset);

    switch (track_asset->track_type) {
    case DEPOT_LEFT:
        if (view.right >= (track->tile_x + track_asset->bitmap_occupancy_x - 1) * 16) {
            depot_state = 4;
            SetVisible(true);
        }
        break;
    case DEPOT_RIGHT:
        if (view.left < (track->tile_x + 1) * 16) {
            depot_state = 4;
            SetVisible(true);
        }
        break;
    case DEPOT_TOP:
        if (view.bottom > (track->tile_y + track_asset->bitmap_occupancy_y - 2) * 16) {
            depot_state = 4;
            SetVisible(true);
        }
        break;
    case DEPOT_BOTTOM:
        if (view.top < (track->tile_y + 1) * 16) {
            depot_state = 4;
            SetVisible(true);
        }
        break;
    default:
        break;
    }

    return false;
}

bool TrainCar::MoveBridge(Train* train) {
    // TODO
    return false;
}
