#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TCSGUICourier.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void switchGUICourier() {
    int SWITCH = WhoIs("SWITCH");
    int GUI = WhoIs("GUI");
    int result;

    char msg[128];
    MessageHeader *mh = (MessageHeader *)msg;
    SWMessage *swmsg = (SWMessage *)msg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get string to print to terminal
        result = Send(SWITCH, (char *)&rdymsg, rdymsg.size(), (char*)&msg, 128);
        if (result < 0) {
            bwprintf(COM2, "Switch->GUI Courier - Send to Switch Server failed\r\n");
        }
        if (mh->type != Constants::MSG::SW) {
            bwprintf(COM2, "Switch->GUI Courier - Expected SW message type but received unexpected message type %d\r\n", mh->type);
        }

        result = Send(GUI, (char*)swmsg, swmsg->size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Switch->GUI Courier - Send to GUI Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Switch->GUI Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
