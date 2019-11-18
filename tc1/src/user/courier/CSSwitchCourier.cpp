#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TermParseCourier.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void csSwitchCourier() {
    int CS = WhoIs("CS");
    int SWITCH = WhoIs("SWITCH");
    int result;

    SWMessage swmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get switch message from command server
        result = Send(CS, (char *)&rdymsg, rdymsg.size(), (char*)&swmsg, swmsg.size());
        if (result < 0) {
            bwprintf(COM2, "CS->Switch Courier - Send to Command Server failed\r\n");
        }
        if (swmsg.mh.type != Constants::MSG::SW) {
            bwprintf(COM2, "CS->Switch Courier - Expected SW message but received unexpected message type %d\r\n", swmsg.mh.type);
        }

        // Send switch message to switch server
        result = Send(SWITCH, (char*)&swmsg, swmsg.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "CS->Switch Courier - Send to Switch Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "CS->Switch Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
