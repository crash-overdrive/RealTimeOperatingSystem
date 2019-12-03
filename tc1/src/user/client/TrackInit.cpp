#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "io/ts7200.h"
#include "user/client/TrackInit.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void trackInit() {
    int SWITCH = WhoIs("SWITCH");
    TextMessage tm;
    SWMessage swmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage gomsg(Constants::MSG::GO);

    int MARKLIN = WhoIs("MARKLIN");
    Send(MARKLIN, (char*)&gomsg, gomsg.size(), (char*)&rdymsg, rdymsg.size());

    swmsg.state = 'C';
    for (int i = 1; i <= 18; ++i) {
        swmsg.sw = (char)i;
        Send(SWITCH, (char*)&swmsg, swmsg.size(), (char*)&rdymsg, rdymsg.size());
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Setup Client - Expected MSG::RDY but received a different response type\r\n");
        }
    }
    swmsg.sw = 153;
    Send(SWITCH, (char*)&swmsg, swmsg.size(), (char*)&rdymsg, rdymsg.size());
    if (rdymsg.mh.type != Constants::MSG::RDY) {
        bwprintf(COM2, "Setup Client - Expected MSG::RDY but received a different response type\r\n");
    }
    swmsg.sw = 155;
    Send(SWITCH, (char*)&swmsg, swmsg.size(), (char*)&rdymsg, rdymsg.size());
    if (rdymsg.mh.type != Constants::MSG::RDY) {
        bwprintf(COM2, "Setup Client - Expected MSG::RDY but received a different response type\r\n");
    }
    swmsg.state = 'S';
    swmsg.sw = 154;
    Send(SWITCH, (char*)&swmsg, swmsg.size(), (char*)&rdymsg, rdymsg.size());
    if (rdymsg.mh.type != Constants::MSG::RDY) {
        bwprintf(COM2, "Setup Client - Expected MSG::RDY but received a different response type\r\n");
    }
    swmsg.sw = 156;
    Send(SWITCH, (char*)&swmsg, swmsg.size(), (char*)&rdymsg, rdymsg.size());
    if (rdymsg.mh.type != Constants::MSG::RDY) {
        bwprintf(COM2, "Setup Client - Expected MSG::RDY but received a different response type\r\n");
    }
    Exit();
}
