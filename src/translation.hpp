#pragma once
#include "eadk/eadkpp.h"
#include "eadk/eadk.h"

#include "textures.h"
#include "game/celeste.h"
#include "game/tilemap.h"

#include <stdarg.h>
#include <math.h>
#include <cstring>
#include <string>
#include <memory>

#define screenW Screen::Width
#define screenH Screen::Height

#define pico8XOrigin 32
#define pico8YOrigin -8
#define pico8Size 128

#define renderScale 2
#define sprtSize 8

extern int pico8XOrgin;
extern int pico8YOrgin;
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

void emuShutDown();

void gameMain();
