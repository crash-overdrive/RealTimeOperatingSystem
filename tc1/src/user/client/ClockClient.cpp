#include "Constants.hpp"
#include "string.h"
#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "io/ts7200.h"
#include "user/client/ClockClient.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TimeMessage.hpp"
#include "user/model/SystemTime.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void clockClient() {
    int clockServerTid = WhoIs("CLOCK SERVER");
    int GUI = WhoIs("GUI");

    SystemTime sysTime;
    TimeMessage timemsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        int ticks = Delay(clockServerTid, 10);
        sysTime.setSystemTime(ticks);

        timemsg.m = sysTime.minutes;
        timemsg.s = sysTime.seconds;
        timemsg.ms = sysTime.tenthOfASecond;

        Send(GUI, (char*)&timemsg, timemsg.size(), (char*)&rdymsg, rdymsg.size());
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Clock Client - Expected MSG::RDY but received response type %d", rdymsg.mh.type);
        }
    }
}
