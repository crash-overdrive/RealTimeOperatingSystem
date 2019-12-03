#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/UART1TXCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/CharMessage.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart1txCourier() {
    const int UART1TX = WhoIs("UART1TX");
    const int MARKLIN = WhoIs("MARKLIN");
    ThinMessage rdymsg(Constants::MSG::RDY);
    CharMessage txmsg;
    MessageHeader *mh = (MessageHeader *)&txmsg;
    int result;

    FOREVER {
        // Get character from marklin server
        Send(MARKLIN, (char*)&rdymsg, rdymsg.size(), (char*)&txmsg, txmsg.size());
        if (mh->type != Constants::MSG::TX) {
            bwprintf(COM2, "UART1TXCourier - Expected MSG::TX but received unexpected msg type");
        }

        // Send the character to UART1
        result = Putc(UART1TX, UART1, txmsg.ch);
        if (result < 0) {
            bwprintf(COM2, "UART1TXCourier - Putc returned \"Invalid Server\"");
        }
    }
}
