#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/client/SensorData.hpp"
#include "user/client/UART1RXCourier.hpp"
#include "user/client/UART1TXCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/CharMessage.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/server/MarklinServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void marklinServer() {
    RegisterAs("MARKLIN");
    int TXC = Create(6, uart1txCourier);
    int RXC = Create(5, uart1rxCourier);
    int SENSOR = Create(8, sensorData);
    bool reading = false;
    bool txcBlocked = false;
    int tid, result;
    char msg[Constants::TerminalServer::MSG_SIZE];

    MessageHeader *mh;
    CharMessage txmsg;
    txmsg.mh.type = Constants::MSG::TX;
    TextMessage *outmsg = (TextMessage *)&msg;

    TextMessage inmsg;
    CharMessage *rxmsg = (CharMessage *)&msg;
    ThinMessage rdymsg;
    rdymsg.mh.type = Constants::MSG::RDY;

    TRMessage *trmsg = (TRMessage *)&msg;
    RVMessage *rvmsg = (RVMessage *)&msg;
    SWMessage *swmsg = (SWMessage *)&msg;

    // TODO(sgaweda): make this a proper constant
    DataStructures::RingBuffer<char, 16> outbuf;

    FOREVER {
        result = Receive(&tid, msg, Constants::TerminalServer::MSG_SIZE);
        if (result < 0) {
            bwprintf(COM2, "Marklin Server - Receive generated negative value!");
        }

        // Check the type of the message received
        mh = (MessageHeader *)&msg;
        if (mh->type == Constants::MSG::RDY) {
            // Send a character any are buffered
            if (!outbuf.empty() && !reading) {
                txmsg.ch = outbuf.pop();
                if (txmsg.ch == Constants::MarklinConsole::REQUEST_5_SENSOR_DATA) {
                    reading = true;
                }
                Reply(tid, (char *)&txmsg, txmsg.size());
                txcBlocked = false;
            } else {
                txcBlocked = true;
            }
        } else if (mh->type == Constants::MSG::TEXT) {
            // Atomically buffer the message
            for (int i = 0; i < outmsg->msglen; ++i) {
                outbuf.push(outmsg->msg[i]);
            }

            // Send a character if possible
            if (!outbuf.empty() && txcBlocked && !reading) {
                txmsg.ch = outbuf.pop();
                Reply(TXC, (char*)&txmsg, txmsg.size());
                txcBlocked = false;
            }

            Reply(tid, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::TR) {
            outbuf.push(trmsg->speed + (trmsg->headlights ? 16 : 0));
            outbuf.push(trmsg->train);

            // Send a character if possible
            if (!outbuf.empty() && txcBlocked && !reading) {
                txmsg.ch = outbuf.pop();
                Reply(TXC, (char*)&txmsg, txmsg.size());
                txcBlocked = false;
            }

            Reply(tid, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RV) {
            outbuf.push(15);
            outbuf.push(rvmsg->train);

            // Send a character if possible
            if (!outbuf.empty() && txcBlocked && !reading) {
                txmsg.ch = outbuf.pop();
                Reply(TXC, (char*)&txmsg, txmsg.size());
                txcBlocked = false;
            }

            Reply(tid, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::SW) {
            if (swmsg->state == 'c' || swmsg->state == 'C') {
                outbuf.push(34);
            } else if (swmsg->state == 's' || swmsg->state == 'S') {
                outbuf.push(33);
            } else {
                bwprintf(COM2, "Marklin Server - Received invalid track state!");
            }

            // Send a character if possible
            if (!outbuf.empty() && txcBlocked && !reading) {
                txmsg.ch = outbuf.pop();
                Reply(TXC, (char*)&txmsg, txmsg.size());
                txcBlocked = false;
            }

            Reply(tid, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RX) {
            inmsg.msg[inmsg.msglen++] = rxmsg->ch;

            // If we've read all the data reply to the sensor client
            if (inmsg.msglen == 10) {
                Reply(SENSOR, (char*)&inmsg, inmsg.size());
                inmsg.msglen = 0;
                reading = false;
            }

            // Let courier know we're ready for next character
            Reply(RXC, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RESET) {
            outbuf.push(Constants::MarklinConsole::SET_RESET_ON_FOR_SENSORS);

            // Send a character if possible
            if (!outbuf.empty() && txcBlocked && !reading) {
                txmsg.ch = outbuf.pop();
                Reply(TXC, (char*)&txmsg, txmsg.size());
                txcBlocked = false;
            }

            // Let sensor client know we're ready for input
            Reply(tid, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::REQUEST) {
            outbuf.push(Constants::MarklinConsole::REQUEST_5_SENSOR_DATA);

            // Send a character if possible
            if (!outbuf.empty() && txcBlocked && !reading) {
                txmsg.ch = outbuf.pop();
                Reply(TXC, (char*)&txmsg, txmsg.size());
                txcBlocked = false;
            }

            // Reply to sensor client will occur when all sensor banks have been read
        } else {
            bwprintf(COM2, "Marklin Server - Recieved message with invalid type!\r\n");
        }
    }
}