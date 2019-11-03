#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/TXMessage.hpp"
#include "user/server/TerminalServer.hpp"
#include "user/server/UART2TXCourier.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void terminalServer() {
    RegisterAs("TERM");
    int courier = Create(6, uart2txCourier);
    bool courierBlocked = false;
    int tid, result;
    char msg[Constants::TerminalServer::MSG_SIZE];
    // char reply[Constants::TerminalServer::RP_SIZE];

    MessageHeader *mh;
    TXMessage txmsg;
    TextMessage *textmsg;

    // int msglen, rplen, ch, result;

    // DataStructures::RingBuffer<char, Constants::TerminalServer::CMD_SIZE> cmdbuf;

    // TODO(sgaweda): make this a proper constant
    DataStructures::RingBuffer<char, 1024> outbuf;

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
        mh = (MessageHeader *)&msg;
        if (mh->type == Constants::MSG::RDY) {
            if (!outbuf.empty()) {
                txmsg.ch = outbuf.pop();
                Reply(tid, (char *)&txmsg, txmsg.size());
                courierBlocked = false;
            } else {
                courierBlocked = true;
            }
        } else if (mh->type == Constants::MSG::TEXT) {
            textmsg = (TextMessage *)&msg;
            textmsg->msg[17] = '\0';
            bwprintf(COM2, "Message arrived with msglen %d and size %d\r\n", textmsg->msglen, result);
            // bwprintf(COM2, "%d,%d,%d,%d,%d\r\n", textmsg->msg[0], textmsg->msg[1], textmsg->msg[2], textmsg->msg[3], textmsg->msg[textmsg->msglen-1]);
            // bwprintf(COM2, "%c,%c,%c,%c,%c\r\n", textmsg->msg[0], textmsg->msg[1], textmsg->msg[2], textmsg->msg[3], textmsg->msg[textmsg->msglen-1]);
            bwprintf(COM2, "%d,%d,%d,%d\r\n", textmsg->msg[0], textmsg->msg[1], textmsg->msg[2], textmsg->msg[3]);
            bwprintf(COM2, "%c,%c,%c,%c\r\n", textmsg->msg[0], textmsg->msg[1], textmsg->msg[2], textmsg->msg[3]);
            bwprintf(COM2, "%s", "This is a test too\r\n");
            for (int i = 0; i < 25; ++i) {
                bwprintf(COM2, "%d\r\n", ((char *)&textmsg)[i]);
            }
            bwprintf(COM2, "%s", textmsg->msg);
            bwprintf(COM2, "THIS IS BETWEEN\r\n");
            for (int i = 0; i < textmsg->msglen; ++i) {
                outbuf.push(textmsg->msg[i]);
            }
            if (!outbuf.empty() && courierBlocked) {
                txmsg.ch = outbuf.pop();
                Reply(courier, (char* )&txmsg, txmsg.size());
                courierBlocked = false;
            }
        } else {
            bwprintf(COM2, "Terminal Server - Recieved message with invalid type!\r\n");
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