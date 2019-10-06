#include "kern/BootLoader.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/client/TimingTasks.hpp"
#include "user/client/RockPaperScissorClient.hpp"
#include "user/client/TestClient.hpp"
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

    tid = Create(3, nameServer);
    tid = Create(1, clockServer);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    
    Exit();
}