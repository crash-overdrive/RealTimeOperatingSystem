#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "io/ts7200.h"
#include "user/client/SwitchSetup.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void switchSetup() {
    int CLOCK = WhoIs("CLOCK SERVER");
    // int TCS = WhoIs("TCS");
    TextMessage tm;
    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage gomsg(Constants::MSG::GO);

    int MARKLIN = WhoIs("MARKLIN");
    Send(MARKLIN, (char*)&gomsg, gomsg.size(), (char*)&rdymsg, rdymsg.size());

    // for (int i = 1; i <= 18; ++i) {
    //     tm.msglen = format(tm.msg, "sw %d C", i);
    //     tm.msg[tm.msglen++] = 0;
    //     Delay(CLOCK, 20);
    //     Send(TCS, (char*)&tm, tm.size(), (char*)&rdymsg, rdymsg.size());
    //     if (rdymsg.mh.type != Constants::MSG::RDY) {
    //         bwprintf(COM2, "Clock Client - Expected MSG::RDY but received a different response type\r\n");
    //     }
    // }
    // tm.msglen = format(tm.msg, "sw %d C", 153);
    // tm.msg[tm.msglen++] = 0;
    // Delay(CLOCK, 20);
    // Send(TCS, (char*)&tm, tm.size(), (char*)&rdymsg, rdymsg.size());
    // if (rdymsg.mh.type != Constants::MSG::RDY) {
    //     bwprintf(COM2, "Clock Client - Expected MSG::RDY but received a different response type\r\n");
    // }
    // tm.msglen = format(tm.msg, "sw %d S", 154);
    // tm.msg[tm.msglen++] = 0;
    // Delay(CLOCK, 20);
    // Send(TCS, (char*)&tm, tm.size(), (char*)&rdymsg, rdymsg.size());
    // if (rdymsg.mh.type != Constants::MSG::RDY) {
    //     bwprintf(COM2, "Clock Client - Expected MSG::RDY but received a different response type\r\n");
    // }
    // tm.msglen = format(tm.msg, "sw %d C", 155);
    // tm.msg[tm.msglen++] = 0;
    // Delay(CLOCK, 20);
    // Send(TCS, (char*)&tm, tm.size(), (char*)&rdymsg, rdymsg.size());
    // if (rdymsg.mh.type != Constants::MSG::RDY) {
    //     bwprintf(COM2, "Clock Client - Expected MSG::RDY but received a different response type\r\n");
    // }
    // tm.msglen = format(tm.msg, "sw %d S", 156);
    // tm.msg[tm.msglen++] = 0;
    // Delay(CLOCK, 20);
    // Send(TCS, (char*)&tm, tm.size(), (char*)&rdymsg, rdymsg.size());
    // if (rdymsg.mh.type != Constants::MSG::RDY) {
    //     bwprintf(COM2, "Clock Client - Expected MSG::RDY but received a different response type\r\n");
    // }
    // Delay(CLOCK, 20);
    Exit();
}
