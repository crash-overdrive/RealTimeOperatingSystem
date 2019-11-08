#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TermTCSCourier.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void termTCSCourier() {
    int TERM = WhoIs("TERM");
    int TCS = WhoIs("TCS");
    int result;

    TextMessage tm;
    ThinMessage rdymsg(Constants::MSG::REQUEST);

    FOREVER {
        // Get string to print to terminal
        result = Send(TERM, (char *)&rdymsg, rdymsg.size(), (char*)&tm, tm.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Term->TCS Courier - Send to Terminal Server failed\r\n");
        }
        if (tm.mh.type != Constants::MSG::TEXT) {
            bwprintf(COM2, "Term->TCS Courier - Expected TEXT message type but received unexpected message type %d\r\n", tm.mh.type);
        }

        // Send send string to terminal
        result = Send(TCS, (char*)&tm, tm.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Term->TCS Courier - Send to Train Control Server failed\r\n");
        }
        if (((char *)&rdymsg)[0] == Constants::Server::ERR) {
            // TODO: ERROR HANDLE? IGNORE? THIS MAY BE ENOUGH
        } else if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Term->TCS Courier - Expected RDY message type but received unexpected message type\r\n");
        }
        rdymsg.mh.type = Constants::MSG::REQUEST;
    }
}
