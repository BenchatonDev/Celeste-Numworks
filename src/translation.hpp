#pragma once
#include "eadk/eadkpp.h"
#include "eadk/eadkpp.h"

#include "textures.h"
#include "game/celeste.h"
#include "game/tilemap.h"

#include <stdarg.h>
#include <math.h>
#include <cstring>
#include <memory>

#define screenW Screen::Width
#define screenH Screen::Height
#define pico8Size 128

#define sprtSize 8

extern uint16_t pico8XOrgin;
extern uint16_t pico8YOrgin;
extern bool screenShake;
extern bool pauseEmu;
extern void *gameState;

// I'm following Lemon's way
// of handling the "emulator"'s
// input because I don't know how
// else I would :/ and it's the
// safest way to go
extern uint16_t emuBtnState;
extern uint16_t lastEmuBtnState;

// Input related variables :
extern EADK::Keyboard::State state;
extern EADK::Keyboard::State lastState;

// Public functions
void emuInit();

void gameMain();
