#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"
#include "user/notifier/UART1RXNotifier.hpp"
#include "user/server/UART1RXServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void uart1rxServer() {
    int tid;
    char msg[Constants::UART1RXServer::MSG_SIZE];
    char reply[Constants::UART1RXServer::RP_SIZE];
    UART uart1 = UART(UART1_BASE);
    DataStructures::RingBuffer<char, Constants::UART1RXServer::BUFFER_SIZE> rbuf;
    DataStructures::RingBuffer<int, Constants::UART1RXServer::BUFFER_SIZE> waitbuf;

    RegisterAs("UART1RX");

    int notifierTid = Create(0, uart1rxNotifier);

    FOREVER {
        Receive(&tid, msg, Constants::UART1RXServer::MSG_SIZE);

        if (tid == notifierTid) {
            // We've been notified that uart1 is ready to be read, so read everything into buffer
            while (!uart1.isRXEmpty()) {
                rbuf.push(uart1.getc());
            }
            reply[0] = Constants::Server::ACK;
            Reply(tid, reply, 1);

            while (!waitbuf.empty() && !rbuf.empty()) {
                tid = waitbuf.pop();
                reply[0] = rbuf.pop();
                Reply(tid, reply, 1);
            }
            uart1.enableRXInterrupt();
        } else {
            // Request is coming from the kernel, so return a character from the read buffer
            if (rbuf.empty()) {
                waitbuf.push(tid);
            } else {
                reply[0] = rbuf.pop();
                Reply(tid, reply, 1);
            }
        }
    }
}
