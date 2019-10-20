#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/client/UART2TXNotifier.hpp"
#include "user/server/UART2TXServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void uart2txServer() {
    // bwprintf(COM2, "UART2TX Server - entered server\n\r");

    // Server variables
    int tid;
    char msg[Constants::UART2TXServer::MSG_SIZE];
    int msglen;
    char reply[Constants::UART2TXServer::RP_SIZE];
    UART uart2 = UART(UART2_BASE);
    DataStructures::RingBuffer<char, Constants::UART2TXServer::BUFFER_SIZE> txbuf;
    DataStructures::RingBuffer<int, Constants::NUM_TASKS> waitbuf;
    DataStructures::RingBuffer<char, Constants::NUM_TASKS> waitbufData;

    RegisterAs("UART2TX");

    int notifierTid = Create(0, uart2txNotifier);
    // bwprintf(COM2, "UART2TX Server - created notifier with tid %d\n\r", notifierTid);

    FOREVER {
        msglen = Receive(&tid, msg, Constants::UART2TXServer::MSG_SIZE);

        if (tid == notifierTid) {
            // We've been notified that uart2 is ready for transmission so flush as much of buffer as possible

            // While uart2 can transmit, push characters
            while (!uart2.isTXFull() && !txbuf.empty()) {
                uart2.putc(txbuf.pop());
            }
            // If we have characters to transmit and uart2 is full, enable transmission interrupts
            if (uart2.isTXFull() && !txbuf.empty()) {
                uart2.enableTXInterrupt();
            }
            reply[0] = Constants::Server::ACK;
            Reply(tid, reply, 1);

            // Move data from the wait buffer to the transmit buffer
            while (!waitbuf.empty() && !txbuf.full()) {
                tid = waitbuf.pop();
                txbuf.push(waitbufData.pop());
                reply[0] = Constants::Server::ACK;
                Reply(tid, reply, 1);
            }
        } else {
            // Request is coming from the kernel, so put the received character on the tx buffer
            if (!txbuf.full()) {
                txbuf.push(msg[0]);
                reply[0] = Constants::Server::ACK;
                Reply(tid, reply, 1);
            } else {
                waitbuf.push(tid);
                waitbufData.push(msg[0]);
            }

            // While uart2 can transmit, push characters
            while (!uart2.isTXFull() && !txbuf.empty()) {
                uart2.putc(txbuf.pop());
            }
            // If we have characters to transmit and uart2 is full, enable transmission interrupts
            if (uart2.isTXFull() && !txbuf.empty()) {
                uart2.enableTXInterrupt();
            }
        }
    }
}