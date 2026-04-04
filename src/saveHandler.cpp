#include "saveHandler.hpp"

#ifdef PLATFORM_DEVICE

// Save file handling area
#define slotNumb 2
#define saveName "CelesteP8.sav"
#define saveSize (sizeof(saveHeader) + slotNumb * stateSize)
#define stateSize Celeste_P8_get_state_size()

// Was the save system initialized ?
// If it wasn't (the init function failed)
// Then none of the save functions will work
bool saveSystemInitiliazed = false;

// Save slot macros for readability
enum SAVE_SLOTS {
    SLOT_MAIN = 1,
    SLOT_BACKUP = 2,
};

// That's the top part of my new save format
// Instead of the raw game state I'll now store
// Some extra data at the top of the save file
// To support new features which need persistant
// Settings. The game name var is just for
// Identification so that is you open the save
// File in a hex editor you know where it's from
struct saveHeader {
    char gameName[22];
    bool screenShake;
    bool saveAuto;
    bool saveLoadAuto;
    bool slot1Valid;
    bool slot2Valid;
};

// It's what we'll use when interacting with the
// Header part of the save file, write the defaults
// to it when creating a new file or retrieving settings
// from the save file :). To make things easier for everyone
// The default saved settings are the ones that are defined
// In translation.cpp or translation.hpp which ever really
saveHeader fileHeader = {
    "Celeste Numworks v1.5",
    screenShake,
    emuAutoSave,
    emuAutoLoad,
    false,
    false,
};

// Common function for all our save file creation needs
int saveFileCreate() {
    void* newFile = malloc(saveSize);
    if (!newFile) { return SAVES_FAIL; }

    // By default slot2 and slot1 are of course empty
    // So we only need to copy the default header data
    // To the temporary buffer we use to create the file
    memcpy(newFile, &fileHeader, sizeof(fileHeader));
    bool fileExist = extapp_fileWrite(saveName, (const char*)newFile, saveSize);
    free(newFile);

    return fileExist ? SAVES_SUCCESS : SAVES_WRITE_FAIL;
}

int savesInit() {
    if (!extapp_fileExists(saveName)) { return saveFileCreate(); }
    else {
        size_t fileLen = 0;
        const char* fileData = extapp_fileRead(saveName, &fileLen);

        if (fileLen == saveSize) {
            memcpy(&fileHeader, (const void*)fileData, sizeof(fileHeader));

            screenShake = fileHeader.screenShake;
            emuAutoSave = fileHeader.saveAuto;
            emuAutoLoad = fileHeader.saveLoadAuto;
            emuAutoSaveFirst = !emuAutoSave;

            saveSystemInitiliazed = true;
        } else {
            // The current save file isn't compatible
            // So we'll hapilly overwrite it, ofc I'll
            // Tell you in the release text :)
            extapp_fileErase(saveName);
            saveSystemInitiliazed = saveFileCreate() == SAVES_SUCCESS ? true : false;
        }

        return saveSystemInitiliazed ? SAVES_SUCCESS : SAVES_FAIL;
    }
}

int fileHeaderUpdate() {
    if (saveSystemInitiliazed) {
        size_t fileLen = 0;
        const char* fileData = extapp_fileRead(saveName, &fileLen);

        fileHeader.screenShake = screenShake;
        fileHeader.saveAuto = emuAutoSave;
        fileHeader.saveLoadAuto = emuAutoLoad;

        memcpy((void*)fileData, &fileHeader, sizeof(fileHeader));
        return SAVES_SUCCESS;
    } else { return SAVES_WRITE_FAIL; }
}

/*
int loadProgressSave() {
    if (!extapp_fileExists(saveName)) { return SAVES_NOTHING_TO_DO; }

    size_t fileLen = 0;
    const char* fileData = extapp_fileRead(saveName, &fileLen);
    void* saveData = (void*)fileData;

    if (!saveData) { return SAVES_READ_FAIL; }

    gameState = malloc(stateSize);
    memcpy(gameState, saveData, stateSize);
    return SAVES_SUCCESS;
}

int writeProgressSave() {
    if (!gameState) { return SAVES_NOTHING_TO_DO; }

    if (extapp_fileExists(saveName)) { extapp_fileErase(saveName); }

    const char* fileData = (char*)gameState;

    if (extapp_fileWrite(saveName, fileData, stateSize))
    { return SAVES_SUCCESS; } else { return SAVES_WRITE_FAIL; }
}*/

#else

/*
int loadProgressSave() {
    return SAVES_NOTHING_TO_DO;
}

int writeProgressSave() {
    return SAVES_NOTHING_TO_DO;
}*/

#endif