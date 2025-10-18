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
    Display::pushRectUniform(Rect(0,0,32,240), 0x00FF00);
    Display::pushRectUniform(Rect(288,0,32,240), 0x00FF00);
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

        // Map : OK (at least on num 0, other untested)
        //emulator(CELESTE_P8_MAP, 0 * 16, 0 * 16, 0, 0, 16, 16, 4);
        // the value of int off need to be 0 to not have flickering
        //emulator(CELESTE_P8_MAP, 0 * 16, 0 * 16, 0, 0, 16, 16, 2);
        //emulator(CELESTE_P8_MAP, 0 * 16,0 * 16, 0, 0, 16, 16, 8);

        // Line and rect
        emulator(CELESTE_P8_LINE, 50, 50, 150, 128, 12);
        emulator(CELESTE_P8_RECTFILL, -4, -4, 2, 2, 7);
        emulator(CELESTE_P8_RECTFILL, 129, 129, 7, 4, 7);
        emulator(CELESTE_P8_RECTFILL, -1, -3, 2, 5, 7);
        emulator(CELESTE_P8_RECTFILL, 128, 112, 6, 5, 7);

        // Maybe flickering is because we're rendering to fast ?
        //Timing::msleep(1000/30);
        // Nope not the actuall problem
    }

    free(gameState);

    return 0;
}
