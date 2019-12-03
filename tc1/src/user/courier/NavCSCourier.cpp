#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/NavCSCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void navCSCourier() {
    int NAV = WhoIs("NAV");
    int CS = WhoIs("CS");
    int result;

    char msg[16];
    MessageHeader *mh = (MessageHeader *)&msg;
    RVMessage *rvmsg = (RVMessage *)&msg;
    SWMessage *swmsg = (SWMessage*)&msg;
    TRMessage *trmsg = (TRMessage *)&msg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get command from NAV Server
        result = Send(NAV, (char *)&rdymsg, rdymsg.size(), msg, 10);
        if (result < 0) {
            bwprintf(COM2, "NAV->CS Courier - Send to Command Server failed\r\n");
        }
        if (mh->type == Constants::MSG::TR) {
            result = Send(CS, (char*)trmsg, trmsg->size(), (char *)&rdymsg, rdymsg.size());
            bwprintf(COM2, "<tr %d %d>\n\r", trmsg->train, trmsg->speed);
        } else if (mh->type == Constants::MSG::RV) {
            result = Send(CS, (char*)rvmsg, rvmsg->size(), (char *)&rdymsg, rdymsg.size());
            bwprintf(COM2, "<rv %d>\n\r", rvmsg->train);
        } else if (mh->type == Constants::MSG::SW) {
            bwprintf(COM2, "<sw %d %c>\n\r", swmsg->sw, swmsg->state);
            result = Send(CS, (char*)swmsg, swmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "NAV->CS Courier - Expected TR,RV,SW message but received unexpected message type %d\r\n", mh->type);
        }

        if (result < 0) {
            bwprintf(COM2, "NAV->CS Courier - Send to Command Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "NAV->CS Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
