#include "../../include/Syscall.hpp"

extern "C" {
	#include <bwio.h>
}

enum SYSCALL {
    CREATE = 2,
    MY_TID,
    PARENT_TID,
    YIELD,
    EXIT
};

int sysCreate(int priority, void (*function)()) {
    volatile int retval;
    // r0 = priority, r1 = function ptr
    asm volatile("stmdb sp!, {r0-r3, r12}");
    asm volatile("swi 2");
    asm volatile("mov r8, r0");
    asm volatile("ldmia sp!, {r0-r3, r12}");
    asm volatile("mov %0, r8" : "=r"(retval));
    return retval;
}

int sysMyTid() {
    volatile int retval;
    asm volatile("stmdb sp!, {r0-r3, r12}");
    asm volatile("swi 3");
    // asm volatile("mov %r0, r0" : "=r"(retval));
    asm volatile("mov r8, r0");
    asm volatile("ldmia sp!, {r0-r3, r12}");
    asm volatile("mov %0, r8" : "=r"(retval));
    return retval;
}

int sysMyParentTid() {
    volatile int retval;
    asm volatile("stmdb sp!, {r0-r3, r12}");
    asm volatile("swi 4");
    // asm volatile("mov %r0, r0" : "=r"(retval));
    asm volatile("mov r8, r0");
    asm volatile("ldmia sp!, {r0-r3, r12}");
    asm volatile("mov %0, r8" : "=r"(retval));
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

int Send(int tid, const char *msg, int msglen, char *reply, int rplen) {
    return 0;
}

int Receive(int *tid, char *msg, int msglen) {
    return 0;
}

int Reply(int tid, const char *reply, int rplen) {
    return 0;
}