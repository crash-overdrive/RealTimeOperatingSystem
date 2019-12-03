#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/notifier/UART1TXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart1txNotifier() {
    char msg[Constants::UART1TXServer::MSG_SIZE];
    char reply[Constants::UART1TXServer::RP_SIZE];
    int rplen, UART1TX;

    UART1TX = WhoIs("UART1TX");

    FOREVER {
        AwaitEvent(Constants::UART1TX_IRQ);

        rplen = Send(UART1TX, msg, 1, reply, 1);
        if (rplen < 0) {
            bwprintf(COM2, "UART1TX Notifier - Failed to send to UART1TX\n\r");
        } else if (rplen == 1 && reply[0] == Constants::Server::ERROR) {
            bwprintf(COM2, "UART1TX Notifier - ERROR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART1TX Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}
