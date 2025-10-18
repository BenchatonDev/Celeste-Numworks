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

        gameMain();
    }

    free(gameState);

    return 0;
}
