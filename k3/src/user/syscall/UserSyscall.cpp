#include "user/syscall/UserSyscall.hpp"
#include "user/syscall/Syscall.hpp"
#include "kern/Message.hpp"
#include "io/bwio.hpp"
#include <cstring>

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
    int response = Send(NAME_SERVER_TID, name, strlen(name), reply, 2);
    if (response > 0) {
        if (reply[0] = 's') {
            return 0;
        }
    }
    return -1;
}

int WhoIs(const char* name) {
    char reply[2];
    int response = Send(NAME_SERVER_TID, name, strlen(name), reply, 1);
    if (response > 0) {
        return (int)reply[0];
    }
    return -1;
}