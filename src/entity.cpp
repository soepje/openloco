#include "entity.h"
#include "rect.h"
#include "globals.h"

Entity::Entity() {
    type = 1;
    SetRect(view, 0, 0, 0, 0);
    ok = true;
    on_frame_set_change = nullptr;
    field_20 = nullptr;
}


void Entity::MarkDirty() {
    // FIXME
}

bool Entity::InView(uint32_t x, uint32_t y) {
    if (view.left <= x && x < view.right && view.top <= y && y < view.bottom) {
        return true;
    }
    return false;
}

void Entity::SetViewPosition(uint32_t x, uint32_t y) {
    MarkDirty();
    SetRect(view, x, y, (view.right - view.left) + x, (view.bottom - view.top) + y);
    MarkDirty();
}

bool Entity::Unk4(uint32_t a, uint32_t b) {
    if (on_frame_set_change != nullptr) {
        on_frame_set_change(a, b);
        return true;
    }
    return false;
}

bool Entity::Unk5(uint32_t a, uint32_t b) {
    if (field_20 != nullptr) {
        field_20(a, b);
        return true;
    }
    return false;
}

ImageEntity::ImageEntity(uint32_t asset_id, int32_t frame_set, uint32_t width_, uint32_t height_) : width(width_), height(height_) {
    if (asset_id > 0) {
        SetAsset(asset_id, frame_set, false);
    }
}

void ImageEntity::SetViewPosition(uint32_t x, uint32_t y) {
    Entity::SetViewPosition(x, y);
    hotspot.x = asset->hotspot_x + x;
    hotspot.y = asset->hotspot_y + y;
    if (field_48 != nullptr) {
        // TODO
    }
}

bool ImageEntity::SetAsset(uint32_t asset_id, int32_t frame_set, bool reload_asset) {
    restart_timestamp = 0;
    if (asset == nullptr || asset->id != asset_id || reload_asset) {
        ok = true;
        if (asset != nullptr) {
            // TODO call something in gamemanager, clear rect?
            delete asset;
            asset = nullptr;
        }
        if (asset_id > 0) {
            asset = ASSET_MANAGER.GetAsset(asset_id);
        }
        if (asset == nullptr) {
            // TODO call something
            ok = false;
            return false;
        }
        asset->LoadBitmap(width, height);
        if (asset->bitmap == nullptr) {
            ok = false;
            return false;
        }

        SetRect(view, view.left, view.top, view.left+asset->frame_width, view.top+asset->frame_height);
        SetRect(asset_mask, 0, 0, asset->frame_width, asset->frame_height);

        current_frameset = -1;
    }

    flags = asset->flags;

    if (frame_set < 0) {
        frame_set = asset->default_frame_set;
    }

    SetSomething(frame_set < 0 ? asset->default_frame_set : frame_set);

    if (current_frameset != -1) {
        return true;
    }

    ok = false;

    return false;
}

void ImageEntity::SetSomething(int32_t frame_set_index) {
    if (field_48 != nullptr) {
        // TODO call something in field_48
        field_5c = 0;
    }

    SetFrameSet(frame_set_index);
}


void ImageEntity::SetFrame(uint32_t frame_index, bool unk) {
    if (!ok) {
        return;
    }

    current_frame = frame_index;
    asset_mask.left = frame_index * asset->frame_width;
    asset_mask.right = (frame_index + 1) * asset->frame_width;

    if (unk) {
        MarkDirty();
    }
}

void ImageEntity::SetVisible(bool visible_) {
    visible = visible_;
    MarkDirty();
    // TODO
}


void ImageEntity::Update() {
    if (!ok) {
        return;
    }

    auto frame_set = &asset->frame_sets[current_frameset];

    if (frame_set->start_frame == frame_set->end_frame && frame_set->next_frame_set < 0) {
        return;
    }

    if (restarting) {
        // if (/* TODO some timing condition && */ frame_set->restart_delay > 0) {
        //     return;
        // }

        if (restart_timestamp > EVENT_MANAGER.time) {
            return;
        }
    }

    int32_t next_frame = 0;
    bool frame_set_end = false;

    if (frame_set->split_frames) {
        animation_counter+=2;
        if (frame_set->start_frame < frame_set->end_frame) {
            next_frame = (frame_set->start_frame + (animation_counter / frame_set->animation_delay)) & 0xfffffffe;
            frame_set_end = next_frame > frame_set->end_frame;
        } else {
            next_frame = (frame_set->start_frame - (animation_counter / frame_set->animation_delay)) & 0xfffffffe;
            frame_set_end = next_frame < frame_set->end_frame;
        }
    } else {
        animation_counter++;
        if (frame_set->start_frame < frame_set->end_frame) {
            next_frame = frame_set->start_frame + (animation_counter / frame_set->animation_delay);
            frame_set_end = next_frame > frame_set->end_frame;
        } else {
            next_frame = frame_set->start_frame - (animation_counter / frame_set->animation_delay);
            frame_set_end = next_frame < frame_set->end_frame;
        }
    }

    if (frame_set_end) {
        next_frame = frame_set->end_frame;
        if (!restarting) {
            restarting = true;
            restart_timestamp = EVENT_MANAGER.time + frame_set->restart_delay;
        } else {
            SetFrameSet(frame_set->next_frame_set);
            restarting = false;
        }
        return; // ??
    }

    if (current_frame != next_frame) {
        SetFrame(next_frame, true);
    }
}

void ImageEntity::DrawFrame(const Rect &rect, bool unk, uint32_t flags) {
    if (asset->bitmap == nullptr || !visible) {
        return;
    }

    Rect dest = {};
    if (!IntersectRect(dest, view, rect)) {
        return;
    }

    flags |= this->flags;

    Rect src = {};

    if (asset->frame_sets[current_frameset].flip) {
        flags |= 0x20;

        SetRect(src,
            (asset_mask.left + view.right) - dest.right,
            dest.top - view.top,
            (asset_mask.right - dest.left) + view.left,
            (asset_mask.bottom - view.bottom) + dest.bottom);


        if (src.right < src.left) {
            std::swap(src.right, src.left);
        }
    } else {
        SetRect(src,
            (asset_mask.left + dest.left) - view.left,
            dest.top - view.top,
            (asset_mask.right - view.right) + dest.right,
            (asset_mask.bottom - view.bottom) + dest.bottom);
    }

    if (unk) {
        flags |= 0x40;
    }

    asset->bitmap->Draw(dest, nullptr, src, flags);
}

void ImageEntity::DrawSplitFrame(const Rect &rect, bool unk, uint32_t flags) {
    if (asset->bitmap == nullptr || !visible) {
        return;
    }

    Rect dest = {};
    if (!IntersectRect(dest, view, rect)) {
        return;
    }

    flags |= this->flags;

    Rect src = {};

    if (asset->frame_sets[current_frameset].flip) {
        flags |= 0x20;

        SetRect(src,
            (asset_mask.left + view.right) - dest.right,
            dest.top - view.top,
            (asset_mask.right - dest.left) + view.left,
            (asset_mask.bottom - view.bottom) + dest.bottom);


        if (src.right < src.left) {
            std::swap(src.right, src.left);
        }
    } else {
        SetRect(src,
            (asset_mask.left + dest.left) - view.left,
            dest.top - view.top,
            (asset_mask.right - view.right) + dest.right,
            (asset_mask.bottom - view.bottom) + dest.bottom);
    }

    SetFrame(current_frame+1, false);

    // Why did we do all the stuff above ???
    SetRect(src,
        asset_mask.left + (dest.left - view.left),
        dest.top - view.top,
        asset_mask.right + (dest.right - view.right),
        asset_mask.bottom + (dest.bottom - view.bottom));

    if (unk) {
        flags |= 0x40;
    }

    asset->bitmap->Draw(dest, nullptr, src, flags);
    SetFrame(current_frame-1, false);
}

void ImageEntity::SetName(const std::string& name) {
    this->name = std::string(name);
}

uint32_t ImageEntity::SetFrameSet(int32_t frame_set_index) {
    if (!ok) {
        return 0;
    }

    if (frame_set_index >= 0 && frame_set_index < asset->number_of_frame_sets) {
        current_frameset = frame_set_index;
        auto frameset = asset->frame_sets[frame_set_index];
        animation_counter = 0;
        restart_timestamp = 0;
        current_frame = frameset.start_frame;
        SetFrame(frameset.start_frame, true);
        // TODO play/load sound from frameset
    }

    return current_frame;
}
