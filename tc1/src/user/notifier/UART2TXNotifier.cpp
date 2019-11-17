#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/notifier/UART2TXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart2txNotifier() {
    // bwprintf(COM2, "UART2TX Notifier - entered server\n\r");

    char msg[Constants::UART2TXServer::MSG_SIZE];
    char reply[Constants::UART2TXServer::RP_SIZE];
    int rplen, result, uart2tx_tid;
    // TODO(sgaweda): add a queue to store queued tasks

    uart2tx_tid = WhoIs("UART2TX");

    FOREVER {
        // bwprintf(COM2, "UART1R Notifier - entered AwaitEvent\n\r");
        result = AwaitEvent(Constants::UART2TX_IRQ);
        // bwprintf(COM2, "UART1R Notifier - exited AwaitEvent\n\r");

        rplen = Send(uart2tx_tid, msg, 1, reply, 1);

        if (rplen == 1 && reply[0] == Constants::Server::ERROR) {
            bwprintf(COM2, "UART2TX Notifier - ERROR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART2TX Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}
