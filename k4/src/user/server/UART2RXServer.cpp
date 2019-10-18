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
    // bwprintf(COM2, "U2C\n\r");
    // Server variables
    int tid;
    char msg[Constants::UART2RXServer::MSG_SIZE];
    int msglen;
    char reply[Constants::UART2RXServer::RP_SIZE];
    UART uart2;

    RegisterAs("UART2RX");

    int notifierTid = Create(0, uart2rxNotifier);
    // bwprintf(COM2, "UART2RX Server - created notifier with tid %d\n\r", notifierTid);

    DataStructures::RingBuffer<char, Constants::UART2RXServer::BUFFER_SIZE> rbuf;

    FOREVER {
        msglen = Receive(&tid, msg, Constants::UART2RXServer::MSG_SIZE);

        if (tid == notifierTid) {
            // bwprintf(COM2, "UART2RX Server - received a message from notifier\n\r");
            volatile int rxfe = *(int *)(UART2_BASE + UART_FLAG_OFFSET) & RXFE_MASK;

            while (!rxfe) {
                rbuf.push(uart2.getc());
                rxfe = *(int *)(UART2_BASE + UART_FLAG_OFFSET) & RXFE_MASK;
            }

            reply[0] = Constants::Server::ACK;
            Reply(tid, reply, 1);
            uart2.enableRXInterrupt(COM2);
            // bwprintf(COM2, "UART2RX Server - re-enabling interrupts\n\r");
        } else {
            // Request is coming from the kernel, so return a character from the read buffer
            reply[0] = rbuf.pop();
            Reply(tid, reply, 1);
        }
    }
}