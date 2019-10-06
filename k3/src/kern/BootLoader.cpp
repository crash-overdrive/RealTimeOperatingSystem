#include "kern/BootLoader.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/client/TimingTasks.hpp"
#include "user/client/RockPaperScissorClient.hpp"
#include "user/client/TestClient.hpp"
#include "user/client/ClockClient.hpp"
#include "user/client/IdleClient.hpp"
#include "user/client/ClockNotifier.hpp"
#include "user/server/RockPaperScissorServer.hpp"
#include "user/server/NameServer.hpp"
#include "user/server/ClockServer.hpp"
#include "io/bwio.hpp"


void bootLoader() {
    int tid;
    // Create(2, mainClient);

    // tid = Create(3, nameServer);
    // tid = Create(1, rockPaperScissorServer);
    // tid = Create(1, rockPaperScissorClient);
    // tid = Create(1, rockPaperScissorClient);
    // tid = Create(1, rockPaperScissorClient);
    // tid = Create(1, rockPaperScissorClient);
    // tid = Create(1, rockPaperScissorClient);
    // tid = Create(1, rockPaperScissorClient);

    // tid = Create(1, receiveTask);
    // tid = Create(2, sendTask);

    // tid = Create(3, nameServer);
    // bwprintf(COM2, "Created Name Server with tid: %d\n\r", tid);
    // tid = Create(3, clockServer);
    // bwprintf(COM2, "Created Clock Server with tid: %d\n\r", tid);
    
    // bwprintf(COM2, "Created Idle Task with tid: %d\n\r", tid);

    tid = Create(1, clockNotifier);
    tid = Create(7, idleClient);

    // tid = Create(1, clockClient);
    // tid = Create(1, clockClient);
    // tid = Create(1, clockClient);
    // tid = Create(1, clockClient);

    // Receive();
    // Receive();
    // Receive();
    // Receive();
    
    Exit();
}