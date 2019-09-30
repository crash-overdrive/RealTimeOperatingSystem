#include "../../include/TimingTasks.hpp"
#include "../../include/Util.hpp"
#include "../../include/UserSyscall.hpp"
#include "../../include/bwio.h"

void sendTask() {
    // int receiveServer = WhoIs("ReceiveServer");
    char msg[14] = "Hello World!\n";
    char reply[32];
    int replySize = 100;
    // replySize = Send(receiveServer, msg, 14, &reply, 32);
    Yield();
    replySize = Send(3, msg, 14, reply, 42);
    bwprintf(COM2, "DEBUG: Size of message returned to SEND is %d with msg %s\n\r", replySize, reply);
    Exit();
}

void receiveTask() {
    int tid = 0;
    char msg[32];
    int msglen = 0;
    int repret = 1;
    char reply[17] = "Hello Sunshine!\n";
    Yield();
    msglen = Receive(&tid, msg, 32);
    bwprintf(COM2, "DEBUG: RECEIVED a message <%s> with length %d from tid %d\n\r", msg, msglen, tid);
    repret = Reply(tid, reply, 17);
    bwprintf(COM2, "DEBUG: Our REPLY returned the code %d and msg %s\n\r", repret, reply);
    Exit();
}