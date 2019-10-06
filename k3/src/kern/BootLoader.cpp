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
#include "Constants.hpp"


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
    bwprintf(COM2, "BootLoader - Created Name Server with tid: %d\n\r", tid);
    tid = Create(1, clockServer);
    bwprintf(COM2, "BootLoader - Created Clock Server with tid: %d\n\r", tid);
    tid = Create(Constants::NUM_PRIORITIES-1, idleClient);
    bwprintf(COM2, "BootLoader - Created Idle Task with tid: %d\n\r", tid);

    tid = Create(3, clockClient);
    // tid = Create(1, clockClient);
    // tid = Create(1, clockClient);
    // tid = Create(1, clockClient);

    // Receive();
    // Receive();
    // Receive();
    // Receive();
    
    Exit();
}