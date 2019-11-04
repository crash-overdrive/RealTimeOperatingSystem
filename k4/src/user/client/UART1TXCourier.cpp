#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/UART1TXCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/CharMessage.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart1txCourier() {
    const int UART1TX = WhoIs("UART1TX");
    const int MARKLIN = WhoIs("MARKLIN");
    MessageHeader * mh;
    ThinMessage readymsg;
    readymsg.mh.type = Constants::MSG::RDY;
    CharMessage txmsg;
    int result;

    FOREVER {
        // Get character from marklin server
        Send(MARKLIN, (char*)&readymsg, readymsg.size(), (char*)&txmsg, txmsg.size());
        mh = (MessageHeader *)&txmsg;
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