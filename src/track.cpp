#include "track.h"
#include "asset.h"


Track::Track(uint32_t resource_id) : Building(resource_id) {
    TrackAsset* track_asset = dynamic_cast<TrackAsset*>(asset);

    type = 4;
    track_type = TrackType::REGULAR;
    track_state = TrackState::DEFAULT;

    if (track_asset->track_type == SWITCH) {
        track_type = TrackType::SWITCH;
        track_state = static_cast<TrackState>(track_asset->default_frame_set);
        SetSomething(track_asset->default_frame_set);

        // NOTE seems to be some unreachable code here
    } else if (track_asset->track_type == POINTS) {
        track_type = TrackType::POINTS;
        track_state = TrackState::CONFIG_B;
    } else if (track_asset->IsTunnel()) {
        track_type = TrackType::TUNNEL;
    } else if (track_asset->track_type == CROSSTRACK) {
        track_type = TrackType::CROSSTRACK;
    } else if (track_asset->track_type == BRIDGE_HORIZONTAL || track_asset->track_type == BRIDGE_VERTICAL) {
        track_type = TrackType::BRIDGE;
    } else if (track_asset->track_type == PATH_X_H || track_asset->track_type == PATH_X_V || track_asset->track_type == ROAD_X_H || track_asset->track_type == ROAD_X_V) {
        track_type = TrackType::CROSSING;
        track_state = TrackState::CONFIG_A;
        SetSomething(1);
    } else {
        int32_t resource_id = asset ? asset->id : -1;
        if (resource_id == 3172 || resource_id == 3174 || resource_id == 3176 || resource_id == 3178) {
            track_type = TrackType::BUFFER;
        }
    }

    field_118 = 0;
    num_trains = 0;
}
