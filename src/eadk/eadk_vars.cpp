#include "eadk_vars.h"
#include "eadkpp.h"

// Provide the actual definitions
const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Celeste";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;
