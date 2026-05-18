#ifndef OPENLOCO_OPENLOCO_H
#define OPENLOCO_OPENLOCO_H

#include <SDL3/SDL.h>

// TODO identify remaining states
enum GameState {
    START_SCREEN = 2,
    WORLD_VIEW = 3,
    WORLD_EDIT = 4,
    POST_OFFICE = 5,
    LIBRARY = 6,
    FACTORY = 7,
};

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern double TIME_DELTA;
extern GameState GAME_STATE;

#endif //OPENLOCO_OPENLOCO_H
