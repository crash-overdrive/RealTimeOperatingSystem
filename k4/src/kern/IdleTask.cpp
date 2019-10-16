/*
 * This kernel program just halts, nothing else. The kernel keeps track of when it is executed to determine idle time.
 */

#include "kern/IdleTask.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"

#define FOREVER for(;;)

void idleTask() {
    *(int *)(TIMER3_BASE + LDR_OFFSET) = 0xFFFFFFFF;
    *(int *)(TIMER3_BASE + CRTL_OFFSET) = ENABLE_MASK | MODE_MASK | CLKSEL_MASK;

    volatile int halt;
    FOREVER {
        halt = *(int *)0x80930008; // Puts the system into idle mode
    }
}
