#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/UART1RXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart1rxNotifier() {
    // *(int *)(UART1_BASE + )

    char msg[Constants::UART1RXServer::MSG_SIZE];
    char reply[Constants::UART1RXServer::RP_SIZE];
    int rplen;

    FOREVER {
        msg[0] = AwaitEvent(Constants::UART1RX_IRQ);

        rplen = Send(Constants::UART1RXServer::TID, msg, 1, reply, 1);
        if (rplen == 1 && reply[0] == Constants::Server::ERR) {
            bwprintf(COM2, "UART1R Notifier - ERR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART1R Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}