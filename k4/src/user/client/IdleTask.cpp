/*
 * This kernel program just halts, nothing else. The kernel keeps track of when it is executed to determine idle time.
 */

#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "io/ts7200.h"
#include "user/client/IdleTask.hpp"
#include "user/message/IdleMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void idleTask() {
    // Initialize TIMER2 for Idle task
    *(int *)(TIMER2_BASE + LDR_OFFSET) = 0xFFFFFFFF;
    *(int *)(TIMER2_BASE + CRTL_OFFSET) = ENABLE_MASK | MODE_MASK | CLKSEL_MASK;

    int GUI = WhoIs("GUI");

    IdleMessage idlemsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        int currIdleTime = 0;
        int prevIdleTime = 0;
        int i = 0;
        for (; i < 10; ) {
            int idleTime = Halt();
            if (idleTime != prevIdleTime) {
                ++i;
                prevIdleTime = idleTime;
                currIdleTime += idleTime;
            }
        }

        idlemsg.integer = currIdleTime * 10 / i / 508;
        idlemsg.fractional = currIdleTime * 1000 / i / 508 % 100;

        Send(GUI, (char*)&idlemsg, idlemsg.size(), (char*)&rdymsg, rdymsg.size());
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Clock Client - Expected MSG::RDY but received a different response type");
        }
    }
}
