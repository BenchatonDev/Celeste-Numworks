#include "eadk/eadk_vars.h"
#include "eadk/eadkpp.h"

// The actual game port THX Lemon :)
#include "translation.hpp"

volatile uint64_t g_millisecond_counter = 0;
extern "C" void SysTick_Handler(void) { g_millisecond_counter++; }

uint64_t my_millis() { return g_millisecond_counter; }

// Saying the namespaces we'll use on a
// semi regularly so it's less of a
// hassle to write common functions etc
using namespace EADK;

bool running = true;
constexpr auto targetDeltaTime = 1000 / 30;

int main(void) {
    // Clearing the screen first
    Display::pushRectUniform(Screen::Rect, 0x000000);
    emuInit();

    //Celeste_P8_update();
    //Celeste_P8_draw();

    while (running) { // You can always exit by double pressing home so \(°-°)/
        auto frameStart = my_millis();
        
        if (state.keyDown(Keyboard::Key::Home)) { running = false; }

        gameMain();
        Display::pushRectUniform(Rect(0,0,32,240), 0x000000);
        Display::pushRectUniform(Rect(288,0,32,240), 0x000000);

        auto frameTime = my_millis() - frameStart;

        if (frameTime < targetDeltaTime) {
            auto sleepTime = targetDeltaTime - frameTime;
            Timing::msleep(static_cast<unsigned int>(sleepTime));
        }
    }

    free(gameState);

    return 0;
}
