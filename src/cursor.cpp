#include "cursor.h"
#include "entity.h"
#include "globals.h"
#include "asset.h"
#include <cstdint>

CursorEntity::CursorEntity() : ImageEntity(-1, -1, 0, 0) {
    visible = true;
    SetState(false, true, false);
    field_88 = -1;
    minifig = nullptr;
    minifig_drag = false;
    mouse_params_old[0] = 0;
    mouse_params_old[1] = 0;
    mouse_params_old[2] = 0;
    mouse_params[0] = 0;
    mouse_params[1] = 0;
    mouse_params[2] = 0;
    // TODO SystemParametersInfoA is this even necessary???
    lbutton_down = false;
    rbutton_down = false;
    lbutton_up = false;
    rbutton_up = false;
    field_e4 = 0;
    field_e5 = 0;
    lbutton_state = 0;
    field_e7 = 0;
    busy_cursor = nullptr;
    busy = false;
    CursorEntity::Update();
}

void CursorEntity::SetState(bool visible, bool show_cursor, bool busy) {

}

void CursorEntity::Update() {
    if (!visible) {
        return;
    }

    ImageEntity::Update();

    bool update_cursor = lbutton_down || lbutton_up || rbutton_down || mouse_move;

    if (this->asset) {
        if (mouse_move) {
            if (HandleMouseMove()) {
                // TODO call something
                mouse_move = false;
            }
        }
    }

    if (update_cursor) {

    }
}

bool CursorEntity::HandleMouseMove() {
    // uint16_t mouse_mov_x = mouse_move_coord

    return false;
}

void CursorEntity::SetCursorAsset(uint32_t resource_id) {
    int32_t current_resource_id = asset ? asset->id : -1;
    if (resource_id != current_resource_id) {
        auto cursor_asset = ASSET_MANAGER.GetAsset(resource_id);
        if (cursor_asset) {
            SetAsset(resource_id, cursor_asset->cursor_frame_set, false);
            if (ok) {
                if (asset->type == CURSOR) {
                    SetViewPosition(field_9c - asset->hotspot_x, field_a0 - asset->hotspot_y);
                    return;
                }
                field_88 = asset ? asset->id : -1;
                SetViewPosition(field_9c, field_a0 - static_cast<TileAsset*>(asset)->total_height);
            }
        }
    }
}
