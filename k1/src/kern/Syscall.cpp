#include "../../include/UserSyscall.hpp"
#include "../../include/Syscall.hpp"


int Create(int priority, void (*function)()) {
    return sysCreate(priority, function);
    // assembly pushes priority and function on stack???
    // save rx, ry
    // set rx = priority
    // set ry = fn pointer
    // call swi,2
    // restore rx, ry
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


int sysCreate(int priority, void (*function)());


int sysMyTid();


int sysMyParentTid();


void sysYield();


void sysExit();
