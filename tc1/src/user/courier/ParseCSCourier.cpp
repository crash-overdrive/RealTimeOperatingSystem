#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TermParseCourier.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void parseCSCourier() {
    int PARSE = WhoIs("PARSE");
    int CS = WhoIs("CS");
    int result;

    char cmd[10];
    MessageHeader *mh = (MessageHeader *)&cmd;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get command message from parse server
        result = Send(PARSE, (char *)&rdymsg, rdymsg.size(), cmd, 10);
        if (result < 0) {
            bwprintf(COM2, "Parse->CS Courier - Send to Parse Server failed\r\n");
        }
        if (mh->type != Constants::MSG::TR && mh->type != Constants::RV && mh->type != Constants::SW) {
            bwprintf(COM2, "Parse->CS Courier - Expected SW message but received unexpected message type %d\r\n", mh->type);
        }

        // Send command message to command server
        result = Send(CS, cmd, 10, (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Parse->CS Courier - Send to Command Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Parse->CS Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
