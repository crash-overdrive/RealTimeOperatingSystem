#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/notifier/UART2TXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart2txNotifier() {
    char msg[Constants::UART2TXServer::MSG_SIZE];
    char reply[Constants::UART2TXServer::RP_SIZE];
    int rplen, UART2TX;

    UART2TX = WhoIs("UART2TX");

    FOREVER {
        AwaitEvent(Constants::UART2TX_IRQ);

        rplen = Send(UART2TX, msg, 1, reply, 1);
        if (rplen < 0) {
            bwprintf(COM2, "UART2TX Notifier - Failed to send to UART2TX\n\r");
        } else if (rplen == 1 && reply[0] == Constants::Server::ERROR) {
            bwprintf(COM2, "UART2TX Notifier - ERROR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART2TX Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}
