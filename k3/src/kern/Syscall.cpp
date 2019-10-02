#include "kern/Syscall.hpp"
#include "kern/Message.hpp"
#include "io/bwio.h"

int sysCreate(int priority, void (*function)()) {
    int retval;
    asm volatile("stmdb sp!, {r1-r3, r12}");
    asm volatile("swi 2");
    // TODO: Determine if we can force this to use a specific register
    asm volatile("mov %0, r0" : "=r"(retval));
    asm volatile("ldmia sp!, {r1-r3, r12}");
    return retval;
}

int sysMyTid() {
    int retval;
    asm volatile("stmdb sp!, {r1-r3, r12}");
    asm volatile("swi 3");
    asm volatile("mov %0, r0" : "=r"(retval));
    asm volatile("ldmia sp!, {r1-r3, r12}");
    return retval;
}

int sysMyParentTid() {
    int retval;
    asm volatile("stmdb sp!, {r1-r3, r12}");
    asm volatile("swi 4");
    asm volatile("mov %0, r0" : "=r"(retval));
    asm volatile("ldmia sp!, {r1-r3, r12}");
    return retval;
}

void sysYield() {
    asm volatile("stmdb sp!, {r0-r3, r12}");
    asm volatile("swi 5");
    asm volatile("ldmia sp!, {r0-r3, r12}");
}

void sysExit() {
    asm volatile("stmdb sp!, {r0-r3, r12}");
    asm volatile("swi 6");
    asm volatile("ldmia sp!, {r0-r3, r12}");
}

int sysSend(SendRequest *sendRequest) {
    int retval;
    asm volatile("stmdb sp!, {r1-r3, r12}");
    asm volatile("mov r0, %0" :: "r"(sendRequest));
    asm volatile("swi 7");
    asm volatile("mov %0, r0" : "=r"(retval));
    asm volatile("ldmia sp!, {r1-r3, r12}");
    return retval;
}

int sysReceive(int *tid, char *msg, int msglen) {
    int retval;
    asm volatile("stmdb sp!, {r1-r3, r12}");
    asm volatile("swi 8");
    asm volatile("mov %0, r0" : "=r"(retval));
    asm volatile("ldmia sp!, {r1-r3, r12}");
    return retval;
}

int sysReply(int tid, const char *reply, int rplen) {
    int retval;
    asm volatile("stmdb sp!, {r1-r3, r12}");
    asm volatile("swi 9");
    asm volatile("mov %0, r0" : "=r"(retval));
    asm volatile("ldmia sp!, {r1-r3, r12}");
    return retval;
}