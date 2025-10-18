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

    while (running) { // You can always exit by double pressing home so \(°-°)/
        if (state.keyDown(Keyboard::Key::Home)) { running = false; }
        gameMain();
        
        Timing::msleep(5000);
    }

    //free(gameState);

    return 0;
}
