#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/GUITermCourier.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void guiTermCourier() {
    int TERM = WhoIs("TERM");
    int GUI = WhoIs("GUI");
    int result;

    TextMessage tm;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get string to print to terminal
        result = Send(GUI, (char *)&rdymsg, rdymsg.size(), (char*)&tm, tm.maxSize());
        if (result < 0) {
            bwprintf(COM2, "GUI->Term Courier - Send to GUI Server failed\r\n");
        }
        if (tm.mh.type != Constants::MSG::TEXT) {
            bwprintf(COM2, "GUI->Term Courier - Expected TEXT message type but received unexpected message type\r\n");
        }

        // Send send string to terminal
        result = Send(TERM, (char*)&tm, tm.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "GUI->Term Courier - Send to Terminal Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "GUI->Term Courier - Expected RDY message type but received unexpected message type\r\n");
        }
    }
}
