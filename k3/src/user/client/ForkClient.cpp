#include "user/client/ForkClient.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"

void daughterClient() {
    int tid, ptid;
    tid = MyTid();
    ptid = MyParentTid();
    bwprintf(COM2, "TestTask: My tid is %d and my parent's tid is %d\n\r", tid, ptid);
    Yield();
    bwprintf(COM2, "TestTask: My tid is %d and my parent's tid is %d\n\r", tid, ptid);
    Exit();
}

void parentClient() {
    int tid;
    tid = Create(3, daughterClient);
    bwprintf(COM2, "MainTask: Created Task: %d\n\r", tid);
    tid = Create(3, daughterClient);
    bwprintf(COM2, "MainTask: Created Task: %d\n\r", tid);
    tid = Create(1, daughterClient);
    bwprintf(COM2, "MainTask: Created Task: %d\n\r", tid);
    tid = Create(1, daughterClient);
    bwprintf(COM2, "MainTask: Created Task: %d\n\r", tid);
    bwprintf(COM2, "MainTask: exiting\n\r");
    Exit();
}