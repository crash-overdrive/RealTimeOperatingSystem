#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/ParseCSCourier.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void parseCSCourier() {
    int PARSE = WhoIs("PARSE");
    int CS = WhoIs("CS");
    int result;

    char cmd[16];
    MessageHeader *mh = (MessageHeader *)&cmd;
    RVMessage *rvmsg = (RVMessage *)&cmd;
    SWMessage *swmsg = (SWMessage *)&cmd;
    TRMessage *trmsg = (TRMessage *)&cmd;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get command message from parse server
        result = Send(PARSE, (char *)&rdymsg, rdymsg.size(), cmd, 10);
        if (result < 0) {
            bwprintf(COM2, "Parse->CS Courier - Send to Parse Server failed\r\n");
        }

        // Send command message to command server
        if (mh->type == Constants::MSG::TR) {
            result = Send(CS, (char*)trmsg, trmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::RV) {
            result = Send(CS, (char*)rvmsg, rvmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::SW) {
            result = Send(CS, (char*)swmsg, swmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "Parse->CS Courier - Expected TR, RV, or SW message but received unexpected message type %d\r\n", mh->type);
        }

        if (result < 0) {
            bwprintf(COM2, "Parse->CS Courier - Send to Command Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Parse->CS Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
