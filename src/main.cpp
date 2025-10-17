#include "eadk/eadk_vars.h"
#include "eadk/eadkpp.h"

// The actual game port THX Lemon :)
#include "translation.hpp"

// Saying the namespaces we'll use on a
// semi regularly so it's less of a
// hassle to write common functions etc
using namespace EADK;
using namespace std; 

// Will be used later for a nice exit but we're testing
// So who cares ?
bool running = true;

int main(void) {
    // Clearing the screen first
    Display::pushRectUniform(Screen::Rect, 0x000000);
    emuInit();

    while (true) { // You can always exit by double pressing home so \(°-°)/
        testFunction();
    }

    return 0;
}
