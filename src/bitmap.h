#ifndef OPENLOCO_BITMAP_H
#define OPENLOCO_BITMAP_H
#include <SDL3/SDL_render.h>
#include <string>

#include <SDL3/SDL_surface.h>

#include "rect.h"





// size 0x20
class Bitmap {
public:
    // 0 = bitmap, 1 = ddraw surface
    int32_t type = 0; // 4
    int32_t width = 0; // 8
    int32_t height = 0; // c

    SDL_Texture* texture = nullptr; // 1c

    void Load(const std::string& resource_name, uint32_t unknown, int32_t width, int32_t height);
    void Draw(const Rect& dest, void* surface, const Rect& src, uint32_t flags);
};

#endif //OPENLOCO_BITMAP_H
