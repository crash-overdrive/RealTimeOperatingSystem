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


int sysCreate(int priority, void (*function)()) {
    // r0 = priority, r1 = function ptr
    asm volatile("stmdb {r0-r3, r12}");
    asm volatile("swi 2");
    asm volatile("ldmai {r0-r3, r12}");
}


int sysMyTid() {
    asm volatile("stmdb {r0-r3, r12}");
    asm volatile("swi 3");
    asm volatile("ldmai {r0-r3, r12}");
}


int sysMyParentTid() {
    asm volatile("stmdb {r0-r3, r12}");
    asm volatile("swi 4");
    asm volatile("ldmai {r0-r3, r12}");
}


void sysYield() {
    asm volatile("stmdb {r0-r3, r12}");
    asm volatile("swi 5");
    asm volatile("ldmai {r0-r3, r12}");
}


void sysExit() {
    asm volatile("stmdb {r0-r3, r12}");
    asm volatile("swi 6");
    asm volatile("ldmai {r0-r3, r12}");
}