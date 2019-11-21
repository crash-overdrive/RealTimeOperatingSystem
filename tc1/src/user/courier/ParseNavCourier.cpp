#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/ParseNavCourier.hpp"
#include "user/message/DTMessage.hpp"
#include "user/message/RTMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TPMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void parseNavCourier() {
    int PARSE = WhoIs("PARSE");
    int NAV = WhoIs("NAV");
    int result;

    char cmd[16];
    MessageHeader *mh = (MessageHeader *)&cmd;
    RTMessage *rtmsg = (RTMessage *)&cmd;
    DTMessage *dtmsg = (DTMessage *)&cmd;
    TPMessage *tpmsg = (TPMessage *)&cmd;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get command message from parse server
        result = Send(PARSE, (char *)&rdymsg, rdymsg.size(), cmd, 10);
        if (result < 0) {
            bwprintf(COM2, "Parse->Nav Courier - Send to Parse Server failed\r\n");
        }

        // Send command message to command server
        if (mh->type == Constants::MSG::RT) {
            result = Send(NAV, (char*)rtmsg, rtmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::DT) {
            result = Send(NAV, (char*)dtmsg, dtmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::TP) {
            result = Send(NAV, (char*)tpmsg, tpmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "Parse->Nav Courier - Expected TR, DT, or TP message but received unexpected message type %d\r\n", mh->type);
        }

        if (result < 0) {
            bwprintf(COM2, "Parse->Nav Courier - Send to Command Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Parse->Nav Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
