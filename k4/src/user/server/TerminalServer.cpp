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

    DataStructures::RingBuffer<char, Constants::TerminalServer::CMD_SIZE> cmdbuf;
    // DataStructures::RingBuffer<char, Constants::TerminalServer::CMD_SIZE> outbuf;

    const int UART2_RX_SERVER = WhoIs("UART2RX");
    const int UART2_TX_SERVER = WhoIs("UART2TX");
    // const int TRAIN_SERVER = WhoIs("TRAIN_SERVER");
    const int TRAIN_SERVER = 10;
    
    FOREVER {
        ch = Getc(UART2_RX_SERVER, UART2);
        // bwprintf(COM1, "CRY\r\n");
        if (ch == Constants::TrainController::ENTER) {
            cmdbuf.push((char)ch);
            msglen = cmdbuf.size();
            for (int i = 0; i < msglen; ++i) {
                msg[i] = cmdbuf.pop();
            }
            result = Send(TRAIN_SERVER, msg, msglen, reply, rplen);
        } else if (ch == Constants::TrainController::BACKSPACE) {
            cmdbuf.pop();
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
            cmdbuf.push((char)ch);
            Putc(UART2_TX_SERVER, UART2, ch);
        }
    }
}