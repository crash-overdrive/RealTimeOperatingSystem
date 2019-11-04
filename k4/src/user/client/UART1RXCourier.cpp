#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/UART1RXCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/CharMessage.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart1rxCourier() {
    const int UART1RX = WhoIs("UART1RX");
    const int MARKLIN = WhoIs("MARKLIN");
    MessageHeader *mh;
    ThinMessage readymsg;
    readymsg.mh.type = Constants::MSG::RDY;
    CharMessage rxmsg;
    rxmsg.mh.type = Constants::MSG::RX;
    int result;

    FOREVER {
        // Get a character from UART1
        result = Getc(UART1RX, UART1);
        if (result < 0) {
            bwprintf(COM2, "UART1RXCourier - Getc returned \"Invalid Server\"");
        } else {
            rxmsg.ch = (char)result;
        }

        // Send the character to marklin server
        Send(MARKLIN, (char*)&rxmsg, rxmsg.size(), (char*)&readymsg, readymsg.size());
        mh = (MessageHeader *)&readymsg;
        if (mh->type != Constants::MSG::RDY) {
            bwprintf(COM2, "UART1RXCourier - Expected MSG::RDY but received unexpected msg type");
        }
    }
}