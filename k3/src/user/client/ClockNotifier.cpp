#include "user/client/ClockNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "Constants.hpp"


#define FOREVER for(;;)

void clockNotifier() {
    bwprintf(COM2, "Clock Notifier - Created Clock notifier\n\r");
    int clockServerTid = WhoIs("wCLOCK SERVER");

    *(int *)(TIMER1_BASE + LDR_OFFSET) = 20; // This is just for testing interrupts
    *(int *)(TIMER1_BASE + CRTL_OFFSET) = ENABLE_MASK | MODE_MASK; // | CLKSEL_MASK;

    // TODO: as part of the the initilization clock notifier should set up the timer interrupts
    char tick[] = "x";
    char replyMessage[2];

    FOREVER {
        // bwprintf(COM2, "Start of loop\n\r");
        int result = AwaitEvent(Constants::TIMER_INTERRUPT);
        // Yield();
        // bwprintf(COM2, "Clock Notifier - Woke up from Await Event Queue\n\r");
        
        // Send to time server
        int replySize = Send(clockServerTid, tick, 2, replyMessage, 2);
        if (!((replySize == 2) && (replyMessage[0] == 'A'))) {
            bwprintf(COM2, "Clock Notifier - Got bad reply: %c\n\r", replyMessage[0]);
        }
    }
}