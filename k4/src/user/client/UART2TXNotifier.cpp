#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/client/UART2TXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart2txNotifier() {
    bwprintf(COM2, "UART2TX Notifier - entered server\n\r");

    char msg[Constants::UART2TXServer::MSG_SIZE];
    msg[0] = Constants::Server::ACK;
    char reply[Constants::UART2TXServer::RP_SIZE];
    int rplen, result, uart2tx_tid;
    UART uart2 = UART(UART2_BASE);
    // TODO(sgaweda): add a queue to store queued tasks

    uart2tx_tid = WhoIs("UART2TX");

    FOREVER {
        rplen = Send(uart2tx_tid, msg, 1, reply, 1);
        uart2.enableTXInterrupt();
        // bwprintf(COM2, "UART1R Notifier - entered AwaitEvent\n\r");
        result = AwaitEvent(Constants::UART2TX_IRQ);
        // bwprintf(COM2, "UART1R Notifier - exited AwaitEvent\n\r");

        // bwprintf(COM2, "Printing: %c\n\r", reply[0]);
        uart2.putc(reply[0]);

        // uart2.disableTXInterrupt();

        // bwprintf(COM2, "SENDING TO UART2TX SERVER\n\r");

        // This should not be reading this, whois should identify what the correct server is at the top of the notifier
        
        // if (rplen == 1 && reply[0] == Constants::Server::ERR) {
        //     bwprintf(COM2, "UART1R Notifier - ERR: %d\n\r", reply[0]);
        // } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
        //     bwprintf(COM2, "UART1R Notifier - Recieved bad reply: %d\n\r", reply[0]);
        // }
    }
}