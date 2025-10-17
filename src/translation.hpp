#pragma once
#include "eadk/eadkpp.h"
#include "eadk/eadkpp.h"

#include "textures.h"
#include "game/celeste.h"
#include "game/tilemap.h"

#include <stdarg.h>
#include <math.h>
#include <cstring>
#include <assert.h>
#include <memory>

#define pico8XOrgin 0
#define pico8YOrgin 0
#define pico8ScreenSize 128

extern int palette[16];
extern bool screenShake;
extern bool pauseEmu;
extern void *gameState;

// I'm following Lemon's way
// of handling the "emulator"'s
// input because I don't know how
// else I would :/ and it's the
// safest way to go
extern int emuBtnState;

// Input related variables :
extern EADK::Keyboard::State state;
extern EADK::Keyboard::State lastState;

// Public functions
void emuInit();

void gameMain();

void testFunction();