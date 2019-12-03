#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/UART2RXCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/CharMessage.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart2rxCourier() {
    const int UART2RX = WhoIs("UART2RX");
    const int TERM = WhoIs("TERM");
    MessageHeader *mh;
    ThinMessage readymsg;
    readymsg.mh.type = Constants::MSG::RDY;
    CharMessage rxmsg;
    rxmsg.mh.type = Constants::MSG::RX;
    int result;

    FOREVER {
        // Get a character from UART2
        result = Getc(UART2RX, UART2);
        if (result < 0) {
            bwprintf(COM2, "UART2RXCourier - Getc returned \"Invalid Server\"");
        } else {
            rxmsg.ch = (char)result;
        }

        // Send the character to terminal server
        Send(TERM, (char*)&rxmsg, rxmsg.size(), (char*)&readymsg, readymsg.size());
        mh = (MessageHeader *)&readymsg;
        if (mh->type != Constants::MSG::RDY) {
            bwprintf(COM2, "UART2RXCourier - Expected MSG::RDY but received unexpected msg type");
        }
    }
}