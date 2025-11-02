#include "eadk/eadk_vars.h"
#include "eadk/eadkpp.h"
// Some functions from eadk.h aren't in eadkpp.h
#include "eadk/eadk.h"

// The actual game port THX Lemon :)
#include "translation.hpp" 

#include "limiter.hpp"
#include "extra.hpp"

// Saying the namespaces we'll use on a
// semi regularly so it's less of a
// hassle to write common functions etc
using namespace EADK;

bool running = true;

// Some vars for the frameLimiter
uint64_t frameEndTime    = 0;
uint64_t frameStartTime  = 0;

int main(void) {
    // Clearing the screen first
    Display::pushRectUniform(Screen::Rect, 0x000000);
    emuInit();

    Point location = Point(20, 20);
    int read = loadProgressSave();
    
    while (running) { // You can always exit by double pressing home so \(°-°)/
        if (state.keyDown(Keyboard::Key::Home)) { running = false; }
        frameStartTime = eadk_timing_millis();

        gameMain();

        #ifdef DEBUG_BUILD
        if (read == EXTRA_READ_FAIL)
        { Display::drawString("Read Error", location, true, eadk_color_black, eadk_color_red); }
        else if (read == EXTRA_NOTHING_TO_DO)
        { Display::drawString("Nothing to Read", location, true, eadk_color_black, eadk_color_red); }
        else
        { Display::drawString("Read Success", location, true, eadk_color_black, eadk_color_red); }
        #endif

        frameEndTime = eadk_timing_millis();

        // Time to limit the frame rate Whoooooo
        frameLimiter(frameEndTime - frameStartTime);
    }
    writeProgressSave();
    emuShutDown();

    return 0;
}
