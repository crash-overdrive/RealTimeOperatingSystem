#include "kern/BootLoader.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/client/TimingTasks.hpp"


void bootLoader() {
    int tid;
    // tid = Create(2, nameServer);
    // tid = Create(2, rpsServer);
    // tid = Create(1, rpsClientPAPER);
    // tid = Create(1, rpsClientROCK);
    // tid = Create(1, rpsClientROCK);
    // tid = Create(1, rpsClientSCISSOR);
    // tid = Create(1, rpsClientPAPER);
    // tid = Create(1, rpsClientSCISSOR);
    tid = Create(1, sendTask);
    tid = Create(1, receiveTask);

    Exit();
}