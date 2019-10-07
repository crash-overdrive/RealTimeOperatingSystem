/*
 * This kernel program just halts, nothing else. The kernel keeps track of when it is executed to determine idle time.
 */

#include "kern/Halt.hpp"

#define FOREVER for(;;)

void halt() {
    volatile int halt;
    FOREVER {
        halt = *(int *)0x80930008; // Puts the system into idle mode
    }
}
