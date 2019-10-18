#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/client/UART2RXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart2rxNotifier() {
    // bwprintf(COM2, "UART2RX Notifier - entered server\n\r");

    char msg[Constants::UART2RXServer::MSG_SIZE];
    char reply[Constants::UART2RXServer::RP_SIZE];
    int rplen, result, uart2rx_tid;
    UART uart2;

    uart2rx_tid = WhoIs("UART2RX");

    FOREVER {
        // bwprintf(COM2, "UART1R Notifier - entered AwaitEvent\n\r");
        result = AwaitEvent(Constants::UART2RX_IRQ);
        // bwprintf(COM2, "UART1R Notifier - exited AwaitEvent\n\r");

        // bwprintf(COM2, "SENDING TO UART2RX SERVER\n\r");

        // This should not be reading this, whois should identify what the correct server is at the top of the notifier
        rplen = Send(uart2rx_tid, msg, 1, reply, 1);
        if (rplen == 1 && reply[0] == Constants::Server::ERR) {
            bwprintf(COM2, "UART1R Notifier - ERR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART1R Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}