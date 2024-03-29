#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/notifier/UART1TXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart1txNotifier() {
    // bwprintf(COM2, "UART1TX Notifier - entered server\n\r");

    char msg[Constants::UART1TXServer::MSG_SIZE];
    char reply[Constants::UART1TXServer::RP_SIZE];
    int rplen, result, uart1tx_tid;
    // TODO(sgaweda): add a queue to store queued tasks

    uart1tx_tid = WhoIs("UART1TX");

    FOREVER {
        // bwprintf(COM2, "UART1R Notifier - entered AwaitEvent\n\r");
        result = AwaitEvent(Constants::UART1TX_IRQ);
        // bwprintf(COM2, "UART1R Notifier - exited AwaitEvent\n\r");

        // bwprintf(COM2, "SENDING TO UART2TX SERVER\n\r");

        rplen = Send(uart1tx_tid, msg, 1, reply, 1);
        // if (rplen < 0) {
        //     bwprintf(COM1, "We have a problem");
        // }

        if (rplen == 1 && reply[0] == Constants::Server::ERR) {
            bwprintf(COM2, "UART1TX Notifier - ERR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART1TX Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}