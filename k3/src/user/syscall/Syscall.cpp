#include "Constants.hpp"
#include "io/bwio.hpp"
#include "kern/Message.hpp"
#include "user/syscall/Syscall.hpp"

int sysCreate(int priority, void (*function)()) {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::CREATE));
    // TODO: Determine if we can force this to use a specific register
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}

int sysMyTid() {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::MY_TID));
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}

int sysMyParentTid() {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::MY_PARENT_TID));
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}

void sysYield() {
    asm volatile("swi %c0" :: "i"(Constants::SWI::YIELD));
}

void sysExit() {
    asm volatile("swi %c0" :: "i"(Constants::SWI::EXIT));
}

int sysSend(SendRequest *sendRequest) {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::SEND));
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}

int sysReceive(int *tid, char *msg, int msglen) {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::RECIEVE));
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}

int sysReply(int tid, const char *reply, int rplen) {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::REPLY));
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}


int sysAwaitEvent(int eventId) {
    int retval;
    int lr;
    asm volatile("mov %0, lr" :: "r"(lr));
    bwprintf(COM2, "Value 2: %d \n\r", lr);
    asm volatile("swi %c0" :: "i"(Constants::SWI::AWAIT_EVENT));
    // bwprintf(COM2, "returned from swi 10 \n\r");
    asm volatile("mov %0, r0" : "=r"(retval));
    asm volatile("mov %0, lr" :: "r"(lr));    
    bwprintf(COM2, "Value 3: %d \n\r", lr);
    return retval;
    // return 42;
    
}

int sysHalt() {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::HALT));
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}