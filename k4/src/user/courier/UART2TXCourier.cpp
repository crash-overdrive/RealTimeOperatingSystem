#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/UART2TXCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/CharMessage.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart2txCourier() {
    const int UART2TX = WhoIs("UART2TX");
    const int TERM = WhoIs("TERM");
    MessageHeader * mh;
    ThinMessage readymsg;
    readymsg.mh.type = Constants::MSG::RDY;
    CharMessage txmsg;
    int result;

    FOREVER {
        // Get character from terminal server
        Send(TERM, (char*)&readymsg, readymsg.size(), (char*)&txmsg, txmsg.size());
        mh = (MessageHeader *)&txmsg;
        if (mh->type != Constants::MSG::TX) {
            bwprintf(COM2, "UART2TXCourier - Expected MSG::TX but received unexpected msg type");
        }

        // Send the character to UART2
        result = Putc(UART2TX, UART2, txmsg.ch);
        if (result < 0) {
            bwprintf(COM2, "UART2TXCourier - Putc returned \"Invalid Server\"");
        }
    }
}