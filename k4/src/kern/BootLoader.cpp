#include "kern/BootLoader.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/client/TimingClient.hpp"
#include "user/client/RockPaperScissorClient.hpp"
#include "user/client/ForkClient.hpp"
#include "user/client/ClockClient.hpp"
#include "user/client/ClockNotifier.hpp"
#include "user/client/ManualTrainControl.hpp"
#include "user/server/RockPaperScissorServer.hpp"
#include "user/server/NameServer.hpp"
#include "user/server/ClockServer.hpp"
#include "io/bwio.hpp"
#include "Constants.hpp"
#include "string.h"

namespace Constants {
    namespace NameServer {
        int TID = -1;
    }
    namespace ClockServer {
        int TID = -1;
    }
}
void bootLoader() {
    int tid;
    // tid = Create(2, daughterClient);

    // tid = Create(1, nameServer);
    // tid = Create(2, rockPaperScissorServer);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);
    // tid = Create(3, rockPaperScissorClient);

    // tid = Create(1, receiveClient);
    // tid = Create(4, sendClient);

    tid = Create(2, nameServer);
    Constants::NameServer::TID = tid;
    // bwprintf(COM2, "BootLoader - Created Name Server with tid: %d\n\r", tid);

    tid = Create(1, clockServer);
    // bwprintf(COM2, "BootLoader - Created Clock Server with tid: %d\n\r", tid);
    Constants::ClockServer::TID = tid;

    tid = Create(3, manualTrainControl);

    // tid = Create(3, clockClient);
    // tid = Create(4, clockClient);
    // tid = Create(5, clockClient);
    // tid = Create(6, clockClient);

    // int sendTid;

    // char sendMessage[2];
    // char replyMessage[8];

    // int t;
    // int n;

    // Receive(&sendTid, sendMessage, 2);
    // t = 10;
    // n = 20;
    // memcpy(replyMessage, &t, sizeof(t));
    // memcpy(replyMessage+4, &n, sizeof(n));
    // Reply(sendTid, replyMessage, 8);

    // Receive(&sendTid, sendMessage, 2);
    // t = 23;
    // n = 9;
    // memcpy(replyMessage, &t, sizeof(t));
    // memcpy(replyMessage+4, &n, sizeof(n));
    // Reply(sendTid, replyMessage, 8);

    // Receive(&sendTid, sendMessage, 2);
    // t = 33;
    // n = 6;
    // memcpy(replyMessage, &t, sizeof(t));
    // memcpy(replyMessage+4, &n, sizeof(n));
    // Reply(sendTid, replyMessage, 8);

    // Receive(&sendTid, sendMessage, 2);
    // t = 71;
    // n = 3;
    // memcpy(replyMessage, &t, sizeof(t));
    // memcpy(replyMessage+4, &n, sizeof(n));
    // Reply(sendTid, replyMessage, 8);
    
    Exit();
}