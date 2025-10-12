#include <stdint.h>
#pragma once

extern const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name")));
extern const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level")));
