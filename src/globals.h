#ifndef OPENLOCO_GLOBALS_H
#define OPENLOCO_GLOBALS_H

#include "asset.h"
#include "building.h"
#include "cursor.h"
#include "entity.h"
#include "world.h"
#include "event.h"
#include "minifig.h"


// DAT_0047e000 contains a list of globals to intialize

// LAB_0045c530 DAT_004a9910
// LAB_0045c560 ASSET_MANAGER
// LAB_0045c590 TRAIN_MANAGER
// LAB_0045c5c0 CURSOR_ENTITY
// LAB_0045c5f0 MINIFIG_MANAGER
// FUN_0045c620 BUILDING_MANAGER
// LAB_0045c650 TIME_MANAGER
// LAB_0045c680 EFFECT_ENTITY_MANAGER
// LAB_0045c6b0 TOYBOX_ENTITY
// LAB_0045c6e0 ZOOMBOX_ENTITY
// LAB_0045c710 MAGNIFYING_GLASS_ENTITY
// FUN_0045c740 WORLD
// LAB_0045c770 DAT_004a9ad0

extern AssetManager ASSET_MANAGER;

extern CursorEntity CURSOR_ENTITY;
extern MinifigManager MINIFIG_MANAGER;
extern BuildingManager BUILDING_MANAGER;
extern EventManager EVENT_MANAGER;

extern World WORLD;

extern BaseAsset* BACKDROP;

#endif //OPENLOCO_GLOBALS_H
