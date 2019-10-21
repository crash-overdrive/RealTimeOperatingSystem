#include "Constants.hpp"
#include "io/bwio.hpp"
#include "kern/Message.hpp"
#include "user/syscall/Syscall.hpp"

int sysCreate(int priority, void (*function)()) {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::CREATE));
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

void sysSwitchOff() {
    asm volatile("swi %c0" :: "i"(Constants::SWI::SWITCH_OFF));
}