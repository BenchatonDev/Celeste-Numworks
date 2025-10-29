#pragma once
// Mostly imported from https://github.com/UpsilonNumworks/Upsilon-External/blob/master/apps/Peanut-GB/speed.c
// With slight tweaks, thanks Yaya.Cout :)

#include "eadk/eadkpp.h"

#include <math.h>
#include <stdint.h>

#define targetDeltaTime 33

void frameLimiter(uint32_t frameTime);