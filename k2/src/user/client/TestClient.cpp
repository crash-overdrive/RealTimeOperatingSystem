#include "user/client/TestClient.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.h"

void testClient() {
    int tid, ptid;
    tid = MyTid();
    ptid = MyParentTid();
    bwprintf(COM2, "TestTask: My tid is %d and my parent's tid is %d\n\r", tid, ptid);
    Yield();
    bwprintf(COM2, "TestTask: My tid is %d and my parent's tid is %d\n\r", tid, ptid);
    Exit();
}

void mainClient() {
    int tid;
    tid = Create(3, testClient);
    bwprintf(COM2, "MainTask: Created Task: %d\n\r", tid);
    tid = Create(3, testClient);
    bwprintf(COM2, "MainTask: Created Task: %d\n\r", tid);
    tid = Create(1, testClient);
    bwprintf(COM2, "MainTask: Created Task: %d\n\r", tid);
    tid = Create(1, testClient);
    bwprintf(COM2, "MainTask: Created Task: %d\n\r", tid);
    bwprintf(COM2, "MainTask: exiting\n\r");
    Exit();
}