#pragma once

#define PLATFORM_DEVICE

#include "translation.hpp"
#include "game/celeste.h"

#ifdef PLATFORM_DEVICE
#include "storage/storage.h"
#endif

// Exit codes for our functions
enum SAVES_EXIT_CODES {
    SAVES_NOTHING_TO_DO = 4,
    SAVES_WRITE_FAIL = 3,
    SAVES_READ_FAIL = 2,
    SAVES_FAIL = 1,
    SAVES_SUCCESS = 0,
};

int savesInit();

int fileHeaderUpdate();

/*
// Saving / loading function stuff ;)
int loadProgressSave();

int writeProgressSave();
*/