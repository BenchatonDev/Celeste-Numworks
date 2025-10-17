#include "eadk/eadk_vars.h"
#include "eadk/eadkpp.h"

// To test out rendering some sprites !
#include "textures.h"

// The actual game port THX Lemon :)
#include "translation.hpp"

#include <stdint.h>
#include <stdbool.h>

// Will be used later for a nice exit but we're testing
// So who cares ?
bool running = true;

int main(void) {
    // Clearing the screen first
    EADK::Display::pushRectUniform(EADK::Screen::Rect, 0x000000);

    #define PixelColor(i,x,y,sprtSheet) defltPallette[sprtSheet[i][y][x]]
    int placingX = -8*2;
    int placingY = 0;

    // Let's draw some sprites ! Only once though
    for (int i = 0; i < 128; i++) {
        // Trying to turn this linear array back into it's
        // 2D Sprite sheet form
        placingX += 8*2;
        if ((i % 16 == 0) && i) {placingX = 0; placingY += 8*2;}

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                EADK::Display::pushRectUniform(EADK::Rect((placingX + 2*x),(placingY + 2*y),2,2), PixelColor(i,x,y,mainSprtSheet));
            }
        }
    }

    placingX = -8*2;
    // Same here, literally copy paste but for the font sheet
    for (int i = 32; i < 160; i++) {
        placingX += 8*2;
        if (((i - 32) % 20 == 0) && i) {placingX = 0; placingY += 8*2;}

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                EADK::Display::pushRectUniform(EADK::Rect((placingX + 2*x),(placingY + 2*y),2,2), PixelColor(i,x,y,fontSprtSheet));
            }
        }
    }

    while (true) { // You can always exit by double pressing home so \(°-°)/
        // Do nothing, no need to redraw the test each time
    }

    return 0;
}
