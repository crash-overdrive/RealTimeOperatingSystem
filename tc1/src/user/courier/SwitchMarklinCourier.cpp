#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/SwitchMarklinCourier.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void switchMarklinCourier() {
    int SWITCH = WhoIs("SWITCH");
    int MARKLIN = WhoIs("MARKLIN");
    int result;

    SWMessage swmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get switch command from switch server
        result = Send(SWITCH, (char *)&rdymsg, rdymsg.size(), (char*)&swmsg, swmsg.size());
        if (result < 0) {
            bwprintf(COM2, "Switch->Marklin Courier - Send to Switch Server failed\r\n");
        }
        if (swmsg.mh.type != Constants::MSG::SW) {
            bwprintf(COM2, "Switch->Marklin Courier - Expected SW message type but received unexpected message type %d %d %d\r\n", swmsg.mh.type, swmsg.sw, swmsg.state);
        }

        // Send switch command to marklin server
        result = Send(MARKLIN, (char*)&swmsg, swmsg.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Switch->Marklin Courier - Send to Marklin Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Switch->Marklin Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
