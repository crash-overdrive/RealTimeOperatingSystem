#include "../../include/MessageTasks.hpp"
#include "../../include/Util.hpp"
#include "../../include/UserSyscall.hpp"

void sendTask() {
    // int receiveServer = WhoIs("ReceiveServer");
    char msg[14] = "Hello World!\n";
    char reply[32];
    int replySize = 0;
    // replySize = Send(receiveServer, msg, 14, &reply, 32);
    replySize = Send(3, msg, 14, reply, 32);
}

void receiveTask() {
    int tid = 0;
    char msg[32];
    int msglen = 0;
    int repret = 1;
    char reply[17] = "Hello Sunshine!\n";
    msglen = Receive(&tid, msg, 32);
    repret = Reply(tid, reply, 17);
}