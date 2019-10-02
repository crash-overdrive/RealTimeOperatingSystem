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