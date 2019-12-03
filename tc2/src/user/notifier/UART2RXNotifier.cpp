#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/notifier/UART2RXNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"


#define FOREVER for(;;)

void uart2rxNotifier() {
    char msg[Constants::UART2RXServer::MSG_SIZE];
    char reply[Constants::UART2RXServer::RP_SIZE];
    int rplen, UART2RX;

    UART2RX = WhoIs("UART2RX");

    FOREVER {
        AwaitEvent(Constants::UART2RX_IRQ);

        rplen = Send(UART2RX, msg, 1, reply, 1);
        if (rplen < 0) {
            bwprintf(COM2, "UART2RX Notifier - Failed to send to UART2RX\n\r");
        } else if (rplen == 1 && reply[0] == Constants::Server::ERROR) {
            bwprintf(COM2, "UART2RX Notifier - ERROR: %d\n\r", reply[0]);
        } else if (!(rplen == 1 && reply[0] == Constants::Server::ACK)) {
            bwprintf(COM2, "UART2RX Notifier - Recieved bad reply: %d\n\r", reply[0]);
        }
    }
}
