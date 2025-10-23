#include "eadk/eadk_vars.h"
#include "eadk/eadkpp.h"
// Some functions from eadk.h aren't in eadkpp.h
#include "eadk/eadk.h"

// The actual game port THX Lemon :)
#include "translation.hpp" 

// Saying the namespaces we'll use on a
// semi regularly so it's less of a
// hassle to write common functions etc
using namespace EADK;

bool running = true;

// Frame rate limiting vars
uint64_t frameStartTime = 0, frameEndTime = 0;
uint64_t targetDeltaTime = 33, frameTime = 0;
uint8_t t = 0;

int main(void) {
    // Clearing the screen first
    Display::pushRectUniform(Screen::Rect, 0x000000);
    emuInit();

    while (running) { // You can always exit by double pressing home so \(°-°)/
        frameEndTime = eadk_timing_millis();
        frameTime = frameEndTime - frameStartTime;

        // Exit if needed else run 1 iteration of the gameLoop
        if (state.keyDown(Keyboard::Key::Home)) { running = false; }

        gameMain();
        
        // Code from Lemon's implementation to cap framerate at exactly 30fps
        // TODO : Make it more dynamic to get back time lost in long frames
        if (t < 2) targetDeltaTime = 33;
	    else       targetDeltaTime = 34;

	    if (t++ == 3) t = 0;

        if (frameTime < targetDeltaTime)
        { Timing::msleep(targetDeltaTime - frameTime); }
        frameStartTime = eadk_timing_millis();
    }
    emuShutDown();

    return 0;
}
