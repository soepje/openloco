#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_timer.h>

#include <cstdint>
#include <ctime>
#include <iostream>

#include "globals.h"
#include "resource.h"
#include "savegame.h"
#include "world.h"
#include "openloco.h"
#include "building.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

double TIME_DELTA = 0.0;

GameState GAME_STATE = WORLD_VIEW;

static int SDLCALL EventLoop(void* data) {
    // TODO something here

    while (GAME_STATE == WORLD_VIEW) {
        EVENT_MANAGER.TriggerEvents();
        SDL_Delay(999);
    }

    return 0;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (argc != 2) {
        SDL_Log("Usage: openloco [lego loco installation directory]");
        return SDL_APP_FAILURE;
    }

    int16_t tiles_x = 64;
    int16_t tiles_y = 48;

    int16_t width = tiles_x * 16;
    int16_t height = tiles_y * 16;

    SDL_SetAppMetadata("openloco", "1.0", "");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("openloco", width, height, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    std::string installation_dir(argv[1]);

    ResourceManager::GetInstance()->Load(installation_dir + "/art-res/resource.");

    BACKDROP = ASSET_MANAGER.GetAsset(1024);
    BACKDROP->LoadBitmap(0, 0);

    WORLD.tiles_x = tiles_x;
    WORLD.tiles_y = tiles_y;

    Savegame s;
    LoadSavegame(installation_dir + "/art-res/SAVEGAME/COW-VILL.SAV", true, true);

    SDL_CreateThread(EventLoop, "event_loop", nullptr);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->user.code == 0x402) {
        Building* building = (Building*) event->user.data1;
        int32_t* resource_id = (int32_t*) event->user.data2;
        building->AddMinifig(*resource_id);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    // the game loop is bound to a timer with a delay of 28 milliseconds
    static uint64_t previous_tick = 0;
    uint64_t current_tick = SDL_GetTicks();
    uint64_t delta = current_tick - previous_tick;
    if (delta < 28) {
        return SDL_APP_CONTINUE;
    }
    previous_tick = current_tick;

    static uint32_t frame_counter = 0;
    static time_t previous_frame = 0;

    SDL_RenderClear(renderer);

    EVENT_MANAGER.UpdateTime();

    BUILDING_MANAGER.Update();
    MINIFIG_MANAGER.Update();

    WORLD.Draw();

    SDL_RenderPresent(renderer);

    frame_counter++;

    // TODO there seems to be some ramping up to 100
    if (frame_counter >= 100) {
        time_t current_frame = std::time(nullptr);
        if (current_frame != previous_frame /* && GAME_STATE == 3 */) {
            TIME_DELTA = (double) frame_counter / (double) (current_frame - previous_frame);
        }
        frame_counter = 0;
        previous_frame = current_frame;
    }


    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {

}
