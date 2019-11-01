#include "user/client/ClockNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "Constants.hpp"


#define FOREVER for(;;)

void clockNotifier() {
    // bwprintf(COM2, "Clock Notifier - Created Clock notifier\n\r");

    *(int *)(TIMER1_BASE + LDR_OFFSET) = Constants::ClockServer::CLOCK_TICK; 
    *(int *)(TIMER1_BASE + CRTL_OFFSET) = ENABLE_MASK | MODE_MASK | CLKSEL_MASK;

    char replyMessage[Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE];
    char sendMessage[1];
    sendMessage[0] = Constants::ClockServer::TICK;

    FOREVER {
        int result = AwaitEvent(Constants::TIMER_1_INTERRUPT);
        
        // Send to time server
        int replySize = Send(Constants::ClockServer::TID, sendMessage, 1, replyMessage, Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE);
        if (!((replySize == 1) && (replyMessage[0] == Constants::ClockServer::ACKNOWLEDGE))) {
            bwprintf(COM2, "Clock Notifier - Got bad reply: %d\n\r", replyMessage[0]);
        }
    }
}