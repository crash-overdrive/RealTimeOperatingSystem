#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/client/UART2RXNotifier.hpp"
#include "user/server/UART2RXServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void uart2rxServer() {
    // bwprintf(COM2, "UART2RX Server - entered server\n\r");

    // Server variables
    int tid;
    char msg[Constants::UART2RXServer::MSG_SIZE];
    int msglen;
    char reply[Constants::UART2RXServer::RP_SIZE];
    UART uart2 = UART(UART2_BASE);
    DataStructures::RingBuffer<char, Constants::UART2RXServer::BUFFER_SIZE> rbuf;
    DataStructures::RingBuffer<int, Constants::UART2RXServer::BUFFER_SIZE> waitbuf;

    RegisterAs("UART2RX");

    int notifierTid = Create(0, uart2rxNotifier);
    // bwprintf(COM2, "UART2RX Server - created notifier with tid %d\n\r", notifierTid);


    FOREVER {
        msglen = Receive(&tid, msg, Constants::UART2RXServer::MSG_SIZE);

        if (tid == notifierTid) {
            while (!uart2.isRXEmpty()) {
                rbuf.push(uart2.getc());
            }

            reply[0] = Constants::Server::ACK;
            Reply(tid, reply, 1);
            if (!waitbuf.empty()) {
                tid = waitbuf.pop();
                reply[0] = rbuf.pop();
                Reply(tid, reply, 1);
            }
            uart2.enableRXInterrupt();
            // bwprintf(COM2, "UART2RX Server - re-enabling interrupts\n\r");
        } else {
            // Request is coming from the kernel, so return a character from the read buffer
            if (rbuf.empty()) {
                waitbuf.push(tid);
                // reply[0] = 0; // If you get 0 just try again Shash, in order for this to be properly blocking, we need a queue of people to repond to and I'm not sure we need it
                // Reply(tid, reply, 1);
            } else {
                reply[0] = rbuf.pop();
                Reply(tid, reply, 1);
            }
        }
    }
}