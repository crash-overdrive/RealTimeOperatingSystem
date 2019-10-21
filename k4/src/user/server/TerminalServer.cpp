#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/server/TerminalServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void terminalServer() {
    int tid;
    char msg[Constants::TerminalServer::MSG_SIZE];
    char reply[Constants::TerminalServer::RP_SIZE];
    int msglen, rplen, ch, result;

    // DataStructures::RingBuffer<char, Constants::TerminalServer::CMD_SIZE> cmdbuf;
    // DataStructures::RingBuffer<char, Constants::TerminalServer::CMD_SIZE> outbuf;

    const int UART2_RX_SERVER = WhoIs("UART2RX");
    const int UART2_TX_SERVER = WhoIs("UART2TX");
    const int TRAIN_SERVER = WhoIs("TRAIN_SERVER");

    FOREVER {
        ch = Getc(UART2_RX_SERVER, UART2);
        if (ch == Constants::TrainController::ENTER) {
            msg[msglen] = (char)ch;
            msglen++;
            Putc(UART2_TX_SERVER, UART2, '\n');
            Putc(UART2_TX_SERVER, UART2, '\r');
            result = Send(TRAIN_SERVER, msg, msglen, reply, rplen);
            msglen = 0;
        } else if (ch == Constants::TrainController::BACKSPACE && msglen > 0) {
            msglen--;
            Putc(UART2_TX_SERVER, UART2, '\033');
            Putc(UART2_TX_SERVER, UART2, '[');
            Putc(UART2_TX_SERVER, UART2, '1');
            Putc(UART2_TX_SERVER, UART2, 'D');
            Putc(UART2_TX_SERVER, UART2, ' ');
            Putc(UART2_TX_SERVER, UART2, '\033');
            Putc(UART2_TX_SERVER, UART2, '[');
            Putc(UART2_TX_SERVER, UART2, '1');
            Putc(UART2_TX_SERVER, UART2, 'D');
        } else {
            msg[msglen] = (char)ch;
            msglen++;
            Putc(UART2_TX_SERVER, UART2, ch);
        }
    }
}