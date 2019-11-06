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
        int totalIdleTime = 0;
        int iterations = 1;
        for (; iterations <= 100;) {
            int idleTime = Halt();
            if (idleTime != 0) {
                ++iterations;
                totalIdleTime = totalIdleTime + idleTime;
            }
        }
        // bwprintf(COM2, "%d.%d\r\n", totalIdleTime * 10 / iterations / 508 , totalIdleTime * 10 / iterations % 508);
        char buffer[50];
        int temp = format(buffer, "\033[s\033[0;10H%d.%d%%\033[u", totalIdleTime * 10 / iterations / 508 , totalIdleTime * 10 / iterations % 508);
        for (int t1 = 0; t1 < temp; ++t1) {
            Putc(UART2_TX, 1, buffer[t1]);
        }
        // if (idleTime != 0) {
        //     bwprintf(COM2, "%d\r\n", idleTime);
        // }
    }
}
