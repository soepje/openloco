#ifndef OPENLOCO_CURSOR_H
#define OPENLOCO_CURSOR_H

#include "entity.h"

class CursorEntity : ImageEntity {
public:
    int32_t field_88; // 88

    uint8_t field_8c; // 8c
    bool busy; // 8d

    bool mouse_move; // 8e
    uint32_t mouse_move_coord; // 90

    uint32_t field_94; // 94
    uint32_t field_98; // 98
    uint32_t field_9c; // 9c
    uint32_t field_a0; // a0

    bool lbutton_down; // a4
    uint32_t lbutton_down_coord; // a8
    uint32_t field_ac; // ac
    uint32_t field_b0; // b0

    bool rbutton_down; // b4
    uint32_t rbutton_down_coord; // b8
    uint32_t field_bc; // bc
    uint32_t field_c0; // c0

    bool lbutton_up; // c4
    uint32_t lbutton_up_coord; // c8
    uint32_t field_cc; // cc
    uint32_t field_d0; // d0

    bool rbutton_up; // d4
    uint32_t rbutton_up_coord; // d8
    uint32_t field_dc; // dc
    uint32_t field_e0; // e0

    uint8_t field_e4; // e4
    uint8_t field_e5; // e5
    bool lbutton_state; // e6
    bool field_e7; // e7

    Entity* minifig; // e8
    bool minifig_drag; // ec

    int32_t mouse_params_old[3]; // f0
    int32_t mouse_params[3]; // fc

    void* busy_cursor; // 108

    // List field_10c; // 10c
    //

    CursorEntity();

    void Update() override;

    bool HandleMouseMove();
    void SetState(bool visible, bool show_cursor, bool busy);
    void SetCursorAsset(uint32_t resource_id);
};

#endif //OPENLOCO_CURSOR_H
