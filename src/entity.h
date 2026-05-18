#ifndef OPENLOCO_ENTITY_H
#define OPENLOCO_ENTITY_H

#include <list>
#include <vector>

#include "asset.h"
#include "rect.h"

class Entity {
public:
    uint32_t type; // 4
    Rect view; // 8
    bool ok; // 18
    uint32_t (*on_frame_set_change)(uint32_t, uint32_t); // 1c
    uint32_t (*field_20)(uint32_t, uint32_t); // 20

    Entity();
    virtual ~Entity() = default; // 0

    virtual void MarkDirty(); // 4
    virtual bool InView(uint32_t x, uint32_t y); // 8
    virtual void SetViewPosition(uint32_t x, uint32_t y); // c
    virtual bool Unk4(uint32_t a, uint32_t b); // 10
    virtual bool Unk5(uint32_t a, uint32_t b); // 14
};

class ImageEntity : public Entity {
public:
    bool visible = true; // 24
    int32_t current_frameset = 0; // 28
    uint32_t flags = 0; // 2c
    Rect asset_mask = {}; // 30 asset clip region
    BaseAsset* asset = nullptr; // 40

    void* field_44 = nullptr; // 44
    void* field_48 = nullptr; // 48

    Point hotspot = {}; // 4c

    uint32_t current_frame = 0; // 54
    uint32_t restart_timestamp = 0; // 58
    int32_t field_5c = 0; // 5c
    uint32_t field_60 = 0; // 60

    uint32_t resource_id = 0; // 64
    uint32_t field_68 = 0; // 68

    uint32_t animation_counter = 0; // 6c
    bool restarting = false; // 70

    // used to resize bitmap dimensions, zero means original bitmap size
    uint32_t width = 0; // 74
    uint32_t height = 0; // 78
    std::string name; // 7c

    ImageEntity(uint32_t asset_id, int32_t frame_set, uint32_t width_, uint32_t height_);

    void SetViewPosition(uint32_t x, uint32_t y) override;

    virtual bool SetAsset(uint32_t asset_id, int32_t frame_set, bool reload_asset); // 18
    virtual void SetSomething(int32_t frame_set_index); // 1c
    virtual void SetFrame(uint32_t frame_index, bool unk); // 20
    virtual void SetVisible(bool visible); // 24
    virtual void Update(); // 28
    virtual void DrawFrame(const Rect& rect, bool unk, uint32_t flags); // 2c
    virtual void DrawSplitFrame(const Rect& rect, bool unk, uint32_t flags); // 30
    virtual void SetName(const std::string& name); // 34
    virtual uint32_t SetFrameSet(int32_t frame_set_index); // 38
};

class EffectEntity : public ImageEntity {

};


#endif //OPENLOCO_ENTITY_H
