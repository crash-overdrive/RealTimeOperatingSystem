#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/TXMessage.hpp"
#include "user/server/TerminalServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void terminalServer() {
    int courier, result, tid;
    char msg[Constants::TerminalServer::MSG_SIZE];
    // char reply[Constants::TerminalServer::RP_SIZE];

    MessageHeader * mh;
    TXMessage txmsg;

    // int msglen, rplen, ch, result;

    // DataStructures::RingBuffer<char, Constants::TerminalServer::CMD_SIZE> cmdbuf;

    // TODO(sgaweda): make this a proper constant
    DataStructures::RingBuffer<char, 1024> outbuf;

    RegisterAs("TERM");
    // const int UART2_RX_SERVER = WhoIs("UART2RX");
    // const int UART2_TX_COURIER = WhoIs("UART2TXC");
    // const int UART2_TX_SERVER = WhoIs("UART2TX");
    // const int TRAIN_SERVER = WhoIs("TRAIN_SERVER");

    FOREVER {
        // TODO(sgaweda): refactor to only use reply
        result = Receive(&tid, msg, Constants::TerminalServer::MSG_SIZE);
        if (result < 0) {
            // TODO: handle empty message
        }

        // Check who we recieved
        mh = (MessageHeader *)msg;
        if (mh->type == Constants::MSG::RDY) {
            if (!outbuf.empty()) {
                Reply(tid, (char *)&txmsg, txmsg.size());
            } else {
                courier = tid;
            }
        } else {
            // TODO(sgaweda): create a properly typed msg
            for (int i = 0; i < result; ++i) {
                outbuf.push(msg[i]);
            }
        }

        // ch = Getc(UART2_RX_SERVER, UART2);
        // if (ch == Constants::TrainController::ENTER) {
        //     msg[msglen] = (char)ch;
        //     msglen++;
        //     Putc(UART2_TX_SERVER, UART2, '\n');
        //     Putc(UART2_TX_SERVER, UART2, '\r');
        //     result = Send(TRAIN_SERVER, msg, msglen, reply, rplen);
        //     msglen = 0;
        // } else if (ch == Constants::TrainController::BACKSPACE && msglen > 0) {
        //     msglen--;
        //     Putc(UART2_TX_SERVER, UART2, '\033');
        //     Putc(UART2_TX_SERVER, UART2, '[');
        //     Putc(UART2_TX_SERVER, UART2, '1');
        //     Putc(UART2_TX_SERVER, UART2, 'D');
        //     Putc(UART2_TX_SERVER, UART2, ' ');
        //     Putc(UART2_TX_SERVER, UART2, '\033');
        //     Putc(UART2_TX_SERVER, UART2, '[');
        //     Putc(UART2_TX_SERVER, UART2, '1');
        //     Putc(UART2_TX_SERVER, UART2, 'D');
        // } else {
        //     msg[msglen] = (char)ch;
        //     msglen++;
        //     Putc(UART2_TX_SERVER, UART2, ch);
        // }
    }
}