/*
 * This kernel program just halts, nothing else. The kernel keeps track of when it is executed to determine idle time.
 */

#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "io/ts7200.h"
#include "user/client/IdleTask.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void idleTask() {
    // Initialize TIMER2 for Idle task
    *(int *)(TIMER2_BASE + LDR_OFFSET) = 0xFFFFFFFF;
    *(int *)(TIMER2_BASE + CRTL_OFFSET) = ENABLE_MASK | MODE_MASK | CLKSEL_MASK;
    int UART2_TX = WhoIs("UART2TX");

    FOREVER {
        int idleTime = Halt();
        char buffer[50];
        int length = format(buffer, "\033[s\033[0;10H%d.%d%%\033[u", idleTime * 10 / 508 , idleTime * 10 % 508);
        for (int i = 0; i < length; ++i) {
            Putc(UART2_TX, 1, buffer[i]);
        }
    }
}
