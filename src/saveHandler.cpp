#include "saveHandler.hpp"

// Save file handling area
#define slotNumb 2
#define saveName "CelesteP8.sav"
#define stateSize Celeste_P8_get_state_size()
#define saveSize (sizeof(saveHeader) + slotNumb * stateSize)

// Was the save system initialized ?
// If it wasn't (the init function failed)
// Then none of the save functions will work
bool saveSystemInitiliazed = false;

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

#ifdef PLATFORM_DEVICE
// The game state to be loaded, it can be
// Either from the backup slot or the main
// Slot, it's simply a buffer to freed right
// After the state is loaded so there is no diff
void* gameState = NULL;

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

            saveSystemInitiliazed = true;
            if (emuAutoLoad) { loadSave(false); }
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

int savesShutDown() { if (gameState) { free(gameState); } saveSystemInitiliazed = false; return SAVES_SUCCESS;}

int fileHeaderUpdate() {
    if (!saveSystemInitiliazed) { return SAVES_FAIL; }
    
    size_t fileLen = 0;
    const char* fileData = extapp_fileRead(saveName, &fileLen);

    fileHeader.screenShake = screenShake;
    fileHeader.saveAuto = emuAutoSave;
    fileHeader.saveLoadAuto = emuAutoLoad;

    memcpy((void*)fileData, &fileHeader, sizeof(fileHeader));
    return SAVES_SUCCESS;
}

bool slotValid(bool backupSlot) {
    return backupSlot ? fileHeader.slot2Valid : fileHeader.slot1Valid;
}

#define slot1Pointer(filePointer) filePointer + sizeof(fileHeader)

int writeSave(bool backupOldSave) {
    if (!saveSystemInitiliazed) { return SAVES_FAIL; }
    if (!emuSaveEnabled) { return SAVES_NOTHING_TO_DO; }

    gameState = malloc(stateSize);
    if (!gameState) { return SAVES_FAIL; }

    size_t fileLen = 0;
    const char* fileData = extapp_fileRead(saveName, &fileLen);

    if (backupOldSave) {
        // This SHOULD copy the data from the main slot to the backup one, hopefully
        memcpy((void*)(slot1Pointer(fileData) + stateSize), (void*)(slot1Pointer(fileData)), stateSize);
        fileHeader.slot2Valid = true;
    }

    // And this should write the new save state to file, again hopefully
    Celeste_P8_save_state(gameState);
    memcpy((void*)(slot1Pointer(fileData)), gameState, stateSize);
    fileHeader.slot1Valid = true;

    free(gameState);
    fileHeaderUpdate();
    return SAVES_SUCCESS;
}

int loadSave(bool backupSlot) {
    if (!saveSystemInitiliazed) { return SAVES_FAIL; }

    gameState = malloc(stateSize);
    if (!gameState) { return SAVES_FAIL; }

    size_t fileLen = 0;
    const char* fileData = extapp_fileRead(saveName, &fileLen);

    if (backupSlot) {
        if ( !fileHeader.slot2Valid ) { free(gameState); return SAVES_NOTHING_TO_DO; } 
        
        memcpy(gameState, (void*)(slot1Pointer(fileData) + stateSize), stateSize);
        Celeste_P8_load_state(gameState);
    } else {
        if ( !fileHeader.slot1Valid ) { free(gameState); return SAVES_NOTHING_TO_DO; }
        
        memcpy(gameState, (void*)(slot1Pointer(fileData)), stateSize);
        Celeste_P8_load_state(gameState);
    }
    
    free(gameState);
    return SAVES_SUCCESS;
}

#undef slot1Pointer

#else

// Two buffers for the saves since memory is far
// from a problem at our scale for the simulators
void* gameState = NULL;
void* backupState = NULL;

int savesInit() { saveSystemInitiliazed = true; return SAVES_SUCCESS; }

int savesShutDown() { free(gameState); free(backupState); saveSystemInitiliazed = false; return SAVES_SUCCESS; }


int writeSave(bool backupOldSave) {
    if (!saveSystemInitiliazed) { return SAVES_FAIL; }
    if (!emuSaveEnabled) { return SAVES_NOTHING_TO_DO; }

    // We still need to check if those exist :/
    gameState = gameState ? gameState : malloc(stateSize);
    backupState = backupState ? backupState : malloc(stateSize);
    if (!gameState) { return SAVES_WRITE_FAIL; }

    // Simple enought !
    if (backupOldSave && fileHeader.slot1Valid == true) {
        if (!backupState) { return SAVES_WRITE_FAIL; } memcpy(backupState, gameState, stateSize);
        fileHeader.slot2Valid = true; }
    
    Celeste_P8_save_state(gameState);
    fileHeader.slot1Valid = true;

    return SAVES_SUCCESS;
}

int loadSave(bool backupSlot) {
    if (!saveSystemInitiliazed) { return SAVES_FAIL; }

    // Still need to know if they exist :/
    if (!gameState) { return SAVES_FAIL; }

    // Even Simpler !
    if (backupSlot) { if (!backupState) { return SAVES_FAIL; }
        if ( !fileHeader.slot2Valid ) { return SAVES_NOTHING_TO_DO; } Celeste_P8_load_state(backupState); }
    else { if ( !fileHeader.slot1Valid ) { return SAVES_NOTHING_TO_DO; } Celeste_P8_load_state(gameState); }
   
    return SAVES_SUCCESS;
}

#endif