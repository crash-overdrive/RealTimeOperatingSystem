#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
// #include "user/courier/TermTCSCourier.hpp"
#include "user/courier/TermParseCourier.hpp"
#include "user/courier/UART2RXCourier.hpp"
#include "user/courier/UART2TXCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/CharMessage.hpp"
#include "user/server/TerminalServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void terminalServer() {
    RegisterAs("TERM");
    int TXC = Create(6, uart2txCourier);
    int RXC = Create(5, uart2rxCourier);
    // int TCS = Create(6, termTCSCourier); // 24
    int PARSE = Create(6, termParseCourier);
    bool txcBlocked = false;
    int tid, result;
    char msg[Constants::TerminalServer::MSG_SIZE];

    MessageHeader *mh = (MessageHeader *)&msg;
    CharMessage txmsg;
    txmsg.mh.type = Constants::MSG::TX;
    TextMessage *outmsg = (TextMessage *)&msg;

    TextMessage inmsg;
    CharMessage *rxmsg = (CharMessage *)&msg;
    ThinMessage rdymsg;
    rdymsg.mh.type = Constants::MSG::RDY;

    // TODO(sgaweda): make this a proper constant
    DataStructures::RingBuffer<char, 1024> outbuf;

    FOREVER {
        // TODO(sgaweda): refactor to only use reply
        result = Receive(&tid, msg, Constants::TerminalServer::MSG_SIZE);
        if (result < 0) {
            // TODO: handle empty message
        }

        // Check the type of the message received
        if (mh->type == Constants::MSG::RDY) {
            // Send a character any are buffered
            if (!outbuf.empty()) {
                txmsg.ch = outbuf.pop();
                Reply(tid, (char *)&txmsg, txmsg.size());
                txcBlocked = false;
            } else {
                txcBlocked = true;
            }
        } else if (mh->type == Constants::MSG::REQUEST) {
            // Reply will be sent when input is finished entering
        } else if (mh->type == Constants::MSG::TEXT) {
            // Atomically buffer the message
            for (int i = 0; i < outmsg->msglen; ++i) {
                outbuf.push(outmsg->msg[i]);
            }
            // Send a character if possible
            if (!outbuf.empty() && txcBlocked) {
                txmsg.ch = outbuf.pop();
                Reply(TXC, (char* )&txmsg, txmsg.size());
                txcBlocked = false;
            }
            Reply(tid, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RX) {
            if (rxmsg->ch == Constants::TrainCommandServer::ENTER) {
                inmsg.msg[inmsg.msglen++] = 0;
                outbuf.push('\033');
                outbuf.push('[');
                outbuf.push('2');
                outbuf.push('1');
                outbuf.push(';');
                outbuf.push('5');
                outbuf.push('f');
                for (int i = 0; i < 40; i++) {
                    outbuf.push(' ');
                }
                outbuf.push('\033');
                outbuf.push('[');
                outbuf.push('2');
                outbuf.push('1');
                outbuf.push(';');
                outbuf.push('5');
                outbuf.push('f');

                Reply(PARSE, (char* )&inmsg, inmsg.size());
                // TODO(sgaweda): Create courier which communicates with train control server
                inmsg.msglen = 0;
            } else if (rxmsg->ch == Constants::TrainCommandServer::BACKSPACE) {
                if (inmsg.msglen > 0) {
                    inmsg.msglen--;
                    outbuf.push('\033');
                    outbuf.push('[');
                    outbuf.push('1');
                    outbuf.push('D');
                    outbuf.push(' ');
                    outbuf.push('\033');
                    outbuf.push('[');
                    outbuf.push('1');
                    outbuf.push('D');
                }
            } else if (rxmsg->ch == 0) {
                // Do nothing
            } else {
                if (inmsg.msglen < 40) {
                    inmsg.msg[inmsg.msglen++] = rxmsg->ch;
                    outbuf.push(rxmsg->ch);
                }
            }

            // Send a character if possible
            if (!outbuf.empty() && txcBlocked) {
                txmsg.ch = outbuf.pop();
                Reply(TXC, (char* )&txmsg, txmsg.size());
                txcBlocked = false;
            }

            // Let courier know we're ready for next character
            Reply(RXC, (char*)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "Terminal Server - Recieved message with invalid type!\r\n");
        }
    }
}
