#pragma once
#include "eadk/eadkpp.h"
#include "eadk/eadkpp.h"

#include "textures.h"
#include "game/celeste.h"
#include "game/tilemap.h"

#include <stdarg.h>
#include <cstring>
#include <assert.h>


#define pico8XOrgin = 0
#define pico8YOrgin = 0

int palette[16];
bool screenShake = true;
bool pauseEmu = false;
void *gameState = NULL;

// I'm following Lemon's way
// of handling the "emulator"'s
// input because I don't know how
// else I would :/ and it's the
// safest way to go
int emuBtnState = 0;

// Input related variables :
EADK::Keyboard::State state = 0;
EADK::Keyboard::State lastState = 0;

// Public functions
void emuInit();

void gameMain();