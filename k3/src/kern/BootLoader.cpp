#include "kern/BootLoader.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/client/TimingTasks.hpp"
#include "user/client/RockPaperScissorClient.hpp"
#include "user/server/RockPaperScissorServer.hpp"
#include "user/server/NameServer.hpp"


void bootLoader() {
    int tid;
    tid = Create(2, nameServer);
    tid = Create(2, rockPaperScissorServer);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    tid = Create(1, rockPaperScissorClient);
    // tid = Create(1, sendTask);
    // tid = Create(1, receiveTask);

    Exit();
}