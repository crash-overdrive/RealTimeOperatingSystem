#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "user/client/UART2RXNotifier.hpp"
#include "user/server/UART2RXServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void uart2rxServer() {
    // bwprintf(COM2, "U2C\n\r");
    // Server variables
    int tid;
    char msg[Constants::UART2RXServer::MSG_SIZE];
    int msglen;
    char reply[Constants::UART2RXServer::RP_SIZE];

    int notifierTid = Create(0, uart2rxNotifier);

    DataStructures::RingBuffer<char, Constants::UART2RXServer::BUFFER_SIZE> rbuf;

    RegisterAs("UART1R SERVER");

    FOREVER {
        msglen = Receive(&tid, msg, Constants::UART2RXServer::MSG_SIZE);
        if (tid == notifierTid) {
            // Get the data and put it in the ringbuffer
            for (int i = 0; i < msglen; ++i) {
                // rbuf.push(msg[i]);
            }
            reply[0] = Constants::Server::ACK;
            Reply(tid, reply, 1);
        } else {
            // Request is coming from the kernel, so return a character from the read buffer
            // reply[0] = rbuf.pop();
            Reply(tid, reply, 1);
        }
    }
}