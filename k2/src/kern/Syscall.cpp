#include "../../include/Syscall.hpp"
#include "../../include/Message.hpp"

enum SYSCALL {
    CREATE = 2,
    MY_TID,
    PARENT_TID,
    YIELD,
    EXIT
};

int sysCreate(int priority, void (*function)()) {
    int retval;
    asm volatile("stmdb sp!, {r1-r3, r12}");
    asm volatile("swi 2");
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

int sysSend(Message message) {
    int retval;
    asm volatile("stmdb sp!, {r1-r3, r12}");
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