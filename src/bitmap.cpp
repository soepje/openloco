#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

#include "openloco.h"
#include "resource.h"
#include "bitmap.h"

void Bitmap::Load(const std::string& name, uint32_t unknown, int32_t width_, int32_t height_) {
    // implementation is refactored to use SDL for bitmap loading

    size_t size = 0;
    auto data = ResourceManager::GetInstance()->Get(name, size);
    if (data == nullptr) {
        return;
    }
    auto iostream = SDL_IOFromMem(data, size);
    if (iostream == nullptr) {
        delete data;
        return;
    }
    auto surface = SDL_LoadBMP_IO(iostream, true);

    SDL_SetSurfaceColorKey(surface, true, SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, 255, 0, 255));

    // TODO resizing if width and height > 0

    if (surface != nullptr) {
        width = surface->w;
        height = surface->h;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);

    delete surface;
    delete data;
}

// flags
// 0x1
// 0x2
// 0x4
// 0x8
// 0x10
// 0x20 flip horizontally
// 0x40
// 0x80
// 0x100
// 0x200
// 0x400

SDL_FRect inline convert_rect(const Rect& in) {
    return SDL_FRect {
        (float) in.left,
        (float) in.top,
        (float) (in.right - in.left),
        (float) (in.bottom - in.top),
    };
}

void Bitmap::Draw(const Rect &dest, void *surface, const Rect &src, uint32_t flags) {
    // TODO
    SDL_FRect s = convert_rect(src);
    SDL_FRect d = convert_rect(dest);

    SDL_FPoint center = { (float) ((src.right - src.left) / 2), 0.0 };

    if (flags & 0x20) {
        SDL_RenderTextureRotated(renderer, texture, &s, &d, 0.0, &center, SDL_FLIP_HORIZONTAL);
    } else {
        SDL_RenderTexture(renderer, texture, &s, &d);
    }



}
