#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/UART2RXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart2rxNotifier() {
    // *(int *)(UART1_BASE + )

    char msg[Constants::UART2RXServer::MSG_SIZE];
    char reply[Constants::UART2RXServer::RP_SIZE];
    int rplen, result;

    FOREVER {
        result = AwaitEvent(Constants::UART1RX_IRQ);

        bwprintf(COM2, "INPUT FROM UART2 DETECTED");

        // This should not be reading this, whois should identify what the correct server is at the top of the notifier
        rplen = Send(Constants::UART2RXServer::TID, msg, 1, reply, 1);
        if (rplen == 1 && reply[0] == Constants::Server::ERR) {
            bwprintf(COM2, "UART1R Notifier - ERR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART1R Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}