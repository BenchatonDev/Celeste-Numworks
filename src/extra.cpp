#include "extra.hpp"

// Save file handling area
#define saveName "CelesteP8.sav"
#define stateSize Celeste_P8_get_state_size()

int loadProgressSave() {
    if (!extapp_fileExists(saveName)) { return EXTRA_NOTHING_TO_DO; }

    size_t fileLen = 0;
    const char* fileData = extapp_fileRead(saveName, &fileLen);
    void* saveData = (void*)fileData;

    if (!saveData) { return EXTRA_READ_FAIL; }

    gameState = malloc(stateSize);
    memcpy(gameState, saveData, stateSize);
    return EXTRA_SUCCESS;
}

int writeProgressSave() {
    if (!gameState) { return EXTRA_NOTHING_TO_DO; }

    if (extapp_fileExists(saveName)) { extapp_fileErase(saveName); }

    const char* fileData = (char*)gameState;

    if (extapp_fileWrite(saveName, fileData, stateSize))
    { return EXTRA_SUCCESS; } else { return EXTRA_WRITE_FAIL; }
}