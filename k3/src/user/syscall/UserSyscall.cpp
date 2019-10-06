#include "user/syscall/UserSyscall.hpp"
#include "user/syscall/Syscall.hpp"
#include "kern/Message.hpp"
#include "io/bwio.hpp"
#include <string.h>

#define NAME_SERVER_TID 1
#define CLOCK_SERVER_TID 2

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
    SendRequest sendRequest;
    sendRequest.tid = tid;
    sendRequest.msg = msg;
    sendRequest.msglen = msglen;
    sendRequest.reply = reply;
    sendRequest.rplen = rplen;
    return sysSend(&sendRequest);
}

int Receive(int *tid, char *msg, int msglen) {
    return sysReceive(tid, msg, msglen);
}

int Reply(int tid, const char *reply, int rplen) {
    return sysReply(tid, reply, rplen);
}

//TODO: find a way to encode the request
int RegisterAs(const char* name) {
    char reply[2];
    int response = Send(NAME_SERVER_TID, name, strlen(name) + 1, reply, 2);
    
    if (response > 0 && reply[0] == 's') {
                    
        return 0;
        
    }
    return -1;
}

int WhoIs(const char* name) {
    char reply[2];
    int response = Send(NAME_SERVER_TID, name, strlen(name) + 1, reply, 2);
    
    if (response > 0 && reply[0] != 'w') {

        return (int)reply[0];

    }
    return -1;
}

int AwaitEvent(int eventId) {
    int lr, retval;
    // asm volatile("mov %0, lr" :: "r"(lr));
    // bwprintf(COM2, "Value 1: %d \n\r", lr);
    // bwprintf(COM2, "Calling sysAwaitEvent\n\r");
    // int response = sysAwaitEvent(eventId);
    // bwprintf(COM2, "Returned from sysAwaitEvent\n\r");
    // asm volatile("mov %0, lr" :: "r"(lr));    
    // bwprintf(COM2, "Value 4: %d \n\r", lr);
    asm volatile("swi 10");
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}