#include "user/client/ClockNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "Constants.hpp"


#define FOREVER for(;;)

void clockNotifier() {
    bwprintf(COM2, "Created Clock notifier\n\r");
    // int clockServerTid = WhoIs("wCLOCK SERVER");
    // TODO: fix this
    int clockServerTid = 2;

    char tick[] = "x";
    char replyMessage[2];

    FOREVER {
        bwprintf(COM2, "Start of loop\n\r");
        int result = AwaitEvent(Constants::TIMER_INTERRUPT);
        // Yield();
        bwprintf(COM2, "Woke up from Await Event Queue\n\r");
        
        // Send to time server
        // int replySize = Send(clockServerTid, tick, 2, replyMessage, 2);
        // if (!((replySize == 2) && (replyMessage[0] == 'a'))) {
        //     bwprintf(COM2, "Got bad reply in clock notifier: %c\n\r", replyMessage[0]);
        // }
    }
}