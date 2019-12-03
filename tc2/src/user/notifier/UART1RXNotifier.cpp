#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/notifier/UART1RXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart1rxNotifier() {
    char msg[Constants::UART1RXServer::MSG_SIZE];
    char reply[Constants::UART1RXServer::RP_SIZE];
    int rplen, UART1RX;

    UART1RX = WhoIs("UART1RX");

    FOREVER {
        msg[0] = AwaitEvent(Constants::UART1RX_IRQ);

        rplen = Send(UART1RX, msg, 1, reply, 1);
        if (rplen < 0) {
            bwprintf(COM2, "UART1RX Notifier - Failed to send to UART1RX\n\r");
        } else if (rplen == 1 && reply[0] == Constants::Server::ERROR) {
            bwprintf(COM2, "UART1RX Notifier - ERROR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART1RX Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}
