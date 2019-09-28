#include "../../include/UserSyscall.hpp"
#include "../../include/Syscall.hpp"

enum SYSCALL {
    CREATE = 2,
    MY_TID,
    PARENT_TID,
    YIELD,
    EXIT
};

int Create(int priority, void (*function)()) {
    return sysCreate(priority, function);
}

int MyTid() {
    return sysMyTid();
}

int MyParentTid() {
    return sysMyParentTid();
}

void Yield() {
    sysYield();
}

void Exit() {
    sysExit();
}

int Send(int tid, const char *msg, int msglen, char *reply, int rplen) {
    // TODO: create struct for passing info into sysSend.
    // sysSend();
    return 0;
}

int Receive(int *tid, char *msg, int msglen) {
    return 0;
}

int Reply(int tid, const char *reply, int rplen) {
    return 0;
}