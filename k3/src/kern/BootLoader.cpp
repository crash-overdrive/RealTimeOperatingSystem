#include "kern/BootLoader.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/client/TimingTasks.hpp"
#include "user/client/RockPaperScissorClient.hpp"
#include "user/client/TestClient.hpp"
#include "user/client/ClockClient.hpp"
#include "user/client/ClockNotifier.hpp"
#include "user/server/RockPaperScissorServer.hpp"
#include "user/server/NameServer.hpp"
#include "user/server/ClockServer.hpp"
#include "io/bwio.hpp"
#include "Constants.hpp"
#include "string.h"


void bootLoader() {
    int tid;
    // Create(2, mainClient);

    // tid = Create(1, nameServer);
    // tid = Create(2, rockPaperScissorServer);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);

    // tid = Create(1, receiveTask);
    // tid = Create(4, sendTask);

    tid = Create(2, nameServer);
    // bwprintf(COM2, "BootLoader - Created Name Server with tid: %d\n\r", tid);
    tid = Create(1, clockServer);
    // bwprintf(COM2, "BootLoader - Created Clock Server with tid: %d\n\r", tid);

    tid = Create(3, clockClient);
    tid = Create(4, clockClient);
    tid = Create(5, clockClient);
    tid = Create(6, clockClient);
    // tid = Create(1, clockClient);
    // tid = Create(1, clockClient);
    // tid = Create(1, clockClient);

    int sendSize;
    int replySize;
    int sendTid;

    char sendMessage[2];
    char replyMessage[8];

    int t;
    int n;

    sendSize = Receive(&sendTid, sendMessage, 2);
    t = 10;
    n = 20;
    memcpy(replyMessage, &t, sizeof(t));
    memcpy(replyMessage+4, &n, sizeof(n));
    Reply(sendTid, replyMessage, 8);

    sendSize = Receive(&sendTid, sendMessage, 2);
    t = 23;
    n = 9;
    memcpy(replyMessage, &t, sizeof(t));
    memcpy(replyMessage+4, &n, sizeof(n));
    Reply(sendTid, replyMessage, 8);

    sendSize = Receive(&sendTid, sendMessage, 2);
    t = 33;
    n = 6;
    memcpy(replyMessage, &t, sizeof(t));
    memcpy(replyMessage+4, &n, sizeof(n));
    Reply(sendTid, replyMessage, 8);

    sendSize = Receive(&sendTid, sendMessage, 2);
    t = 71;
    n = 3;
    memcpy(replyMessage, &t, sizeof(t));
    memcpy(replyMessage+4, &n, sizeof(n));
    Reply(sendTid, replyMessage, 8);
    
    Exit();
}