#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TermParseCourier.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void termParseCourier() {
    int TERM = WhoIs("TERM");
    int PARSE = WhoIs("PARSE");
    int result;

    TextMessage tm;
    ThinMessage rdymsg(Constants::MSG::REQUEST);

    FOREVER {
        // Get string to print to terminal
        result = Send(TERM, (char *)&rdymsg, rdymsg.size(), (char*)&tm, tm.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Term->Parse Courier - Send to Terminal Server failed\r\n");
        }
        if (tm.mh.type != Constants::MSG::TEXT) {
            bwprintf(COM2, "Term->Parse Courier - Expected TEXT message type but received unexpected message type %d\r\n", tm.mh.type);
        }

        // Send send string to terminal
        result = Send(PARSE, (char*)&tm, tm.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Term->Parse Courier - Send to Parse Server failed\r\n");
        }
        if (rdymsg.mh.type == Constants::MSG::ERR) {
            bwprintf(COM2, "Term->Parse Courier - Invalid command error received\r\n");
            rdymsg.mh.type = Constants::MSG::RDY;
        } else if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Term->Parse Courier - Expected RDY message type but received unexpected message type\r\n");
        }
        rdymsg.mh.type = Constants::MSG::REQUEST;
    }
}
