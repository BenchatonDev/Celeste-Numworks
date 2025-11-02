#pragma once
// This file is where I shove functions I don't know how to
// name other than extra :/ It's where saving / loading to
// the "persistant" storage of the calculator and more.

#include <cstring>

#include "translation.hpp"
#include "storage/storage.h"

// Exit codes for our functions
#define EXTRA_NOTHING_TO_DO 2
#define EXTRA_WRITE_FAIL 1
#define EXTRA_READ_FAIL 1
#define EXTRA_SUCCESS 0

// Saving / loading function stuff ;)
int loadProgressSave();

int writeProgressSave();