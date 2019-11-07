#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TCSMarklinCourier.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void tcsMarklinCourier() {
    int MARKLIN = WhoIs("MARKLIN");
    int TCS = WhoIs("TCS");
    int result;

    // TextMessage tm;
    char msg[128];
    MessageHeader *mh = (MessageHeader *)msg;
    SWMessage *swmsg = (SWMessage *)msg;
    TRMessage *trmsg = (TRMessage *)msg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get string to print to terminal
        result = Send(TCS, (char *)&rdymsg, rdymsg.size(), (char*)&msg, 128);
        if (result < 0) {
            bwprintf(COM2, "TCS->Marklin Courier - Send to TCS Server failed\r\n");
        }

        if (mh->type == Constants::MSG::TR) {
            result = Send(MARKLIN, (char*)trmsg, trmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::SW) {
            result = Send(MARKLIN, (char*)swmsg, swmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "TCS->Marklin Courier - Expected TEXT message type but received unexpected message type\r\n");
        }

        if (result < 0) {
            bwprintf(COM2, "TCS->Marklin Courier - Send to Marklin Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "TCS->Marklin Courier - Expected RDY message type but received unexpected message type\r\n");
        }
    }
}
