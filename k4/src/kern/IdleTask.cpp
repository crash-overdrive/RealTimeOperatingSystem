/*
 * This kernel program just halts, nothing else. The kernel keeps track of when it is executed to determine idle time.
 */

#include "kern/IdleTask.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"

#define FOREVER for(;;)

extern "C" int Halt();

void idleTask() {
    *(int *)(TIMER3_BASE + LDR_OFFSET) = 0xFFFFFFFF;
    *(int *)(TIMER3_BASE + CRTL_OFFSET) = ENABLE_MASK | MODE_MASK | CLKSEL_MASK;

    FOREVER {
        int idleTime = Halt();
        bwprintf(COM2, "Idle Time: %d.%d\n\r", idleTime * 10 / 508, idleTime * 10 % 508);
    }
}
