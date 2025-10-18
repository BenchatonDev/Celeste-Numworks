#include "eadk/eadk_vars.h"
#include "eadk/eadkpp.h"

// The actual game port THX Lemon :)
#include "translation.hpp"

// Saying the namespaces we'll use on a
// semi regularly so it's less of a
// hassle to write common functions etc
using namespace EADK;
using namespace std; 

bool running = true;

int main(void) {
    // Clearing the screen first
    Display::pushRectUniform(Screen::Rect, 0x000000);
    emuInit();

    //Celeste_P8_update();
    //Celeste_P8_draw();

    while (running) { // You can always exit by double pressing home so \(°-°)/
        if (state.keyDown(Keyboard::Key::Home)) { running = false; }

        // These work fine
        //emulator(CELESTE_P8_CIRCFILL, 20, 20, 2, 12);
        //emulator(CELESTE_P8_CIRCFILL, 40, 40, 3, 10);

        // Works too
        //emulator(CELESTE_P8_PAL, 8, 11);
        //emulator(CELESTE_P8_SPR, 5, 40, 40, 1, 1, false, false);
        //emulator(CELESTE_P8_PAL_RESET);
        //emulator(CELESTE_P8_SPR, 20, 20, 20, 1, 1, false, true);

        // Printing : OK
        //emulator(CELESTE_P8_PRINT, "Hello World !", 20, 20, 7);
        //emulator(CELESTE_P8_PRINT, "This works nicely !", 40, 40, 7);

        // Weird flickering here on right edges
        emulator(CELESTE_P8_MAP, 0 * 16, 0 * 16, 0, 0, 16, 16, 4);
        emulator(CELESTE_P8_MAP, 0 * 16, 0 * 16, 1, 0, 16, 16, 2);
    }

    free(gameState);

    return 0;
}
