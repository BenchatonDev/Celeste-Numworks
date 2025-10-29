#include "limiter.hpp"
// Mostly imported from https://github.com/UpsilonNumworks/Upsilon-External/blob/master/apps/Peanut-GB/speed.c
// With slight tweaks, thanks Yaya.Cout :)

uint32_t timeBudget = 0;

void frameLimiter(uint32_t frameTime) {

    uint32_t differenceToTarget = abs(targetDeltaTime - (int32_t)frameTime);
    if (targetDeltaTime - frameTime > 0) {
        // Frame was faster than target, so let's slow down if we have time to
        // catch up

        if (timeBudget >= differenceToTarget) {
            // We were too slow at previous frames so we have to catch up
            timeBudget -= differenceToTarget;
        } else if (timeBudget > 0) {
            // We can catch up everything on one frame, so let's sleep a bit less
            // than what we would have done if we weren't late
            uint32_t timeToSleep = differenceToTarget - timeBudget;
            EADK::Timing::msleep(timeToSleep);
            timeBudget = 0;
        } else {
            // We don't have time to catch up, so we just sleep until we get to 16ms/f
            EADK::Timing::msleep(differenceToTarget);
        }
    } else {
        // Frame was slower than target, so we need to catch up.
        timeBudget += differenceToTarget;

        if (timeBudget >= targetDeltaTime) {
            timeBudget = targetDeltaTime;
        }
    }
}