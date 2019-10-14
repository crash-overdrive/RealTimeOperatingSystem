#include "user/client/ClockNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "Constants.hpp"


#define FOREVER for(;;)

void clockNotifier() {
    // bwprintf(COM2, "Clock Notifier - Created Clock notifier\n\r");

    *(int *)(TIMER1_BASE + LDR_OFFSET) = Constants::CLOCK_TICK; 
    *(int *)(TIMER1_BASE + CRTL_OFFSET) = ENABLE_MASK | MODE_MASK | CLKSEL_MASK;

    char tick[] = "x";
    char replyMessage[2];

    FOREVER {
        int result = AwaitEvent(Constants::TIMER_INTERRUPT);
        // bwprintf(COM2, "Clock Notifier - Woke up from Await Event Queue\n\r");
        
        // Send to time server
        int replySize = Send(Constants::ClockServer::TID, tick, 2, replyMessage, 2);
        if (!((replySize == 2) && (replyMessage[0] == 'A'))) {
            bwprintf(COM2, "Clock Notifier - Got bad reply: %c\n\r", replyMessage[0]);
        }
    }
}