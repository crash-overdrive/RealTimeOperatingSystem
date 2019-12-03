#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/notifier/UART1TXNotifier.hpp"
#include "user/server/UART1TXServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void uart1txServer() {
    int tid;
    char msg[Constants::UART1TXServer::MSG_SIZE];
    char reply[Constants::UART1TXServer::RP_SIZE];
    int result;
    UART uart1 = UART(UART1_BASE);
    DataStructures::RingBuffer<char, Constants::UART1TXServer::BUFFER_SIZE> txbuf;
    DataStructures::RingBuffer<int, Constants::NUM_TASKS> waitbuf;
    DataStructures::RingBuffer<char, Constants::NUM_TASKS> waitbufData;
    bool blocked = false;

    RegisterAs("UART1TX");

    int notifierTid = Create(0, uart1txNotifier);

    FOREVER {
        Receive(&tid, msg, Constants::UART1TXServer::MSG_SIZE);

        if (tid == notifierTid) {
            // We've been notified that uart1 is ready for transmission so flush as much of buffer as possible
            blocked = false;
            reply[0] = Constants::Server::ACK;
            result = Reply(tid, reply, 1);
            if (result < 0) {
                bwprintf(COM2, "UART1TXServer - An error was returned from Reply");
            }

            // While uart1 can transmit, push characters
            if (uart1.isTXEmpty() &&  !txbuf.empty()) {
                uart1.putc(txbuf.pop());
                blocked = true;
                uart1.enableTXInterrupt();
            }

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

            // While uart1 can transmit, push characters
            if (!blocked && !uart1.isTXFull() && !txbuf.empty()) {
                uart1.putc(txbuf.pop());
                uart1.enableTXInterrupt();
                blocked = true;
            }
        }
    }
}
