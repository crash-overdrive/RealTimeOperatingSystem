#include "Constants.hpp"
#include "io/bwio.hpp"
#include "kern/Message.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/syscall/Syscall.hpp"
#include <string.h>

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
    int response = Send(Constants::NAME_SERVER_TID, name, strlen(name) + 1, reply, 2);
    
    if (response > 0 && reply[0] == 's') {
                    
        return 0;
        
    }
    return -1;
}

int WhoIs(const char* name) {
    char reply[2];
    int response = Send(Constants::NAME_SERVER_TID, name, strlen(name) + 1, reply, 2);
    
    if (response > 0 && reply[0] != 'w') {

        return (int)reply[0];

    }
    return -1;
}

int AwaitEvent(int eventId) {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::AWAIT_EVENT));
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}

int Time(int tid) {
    if (tid != Constants::CLOCK_SERVER_TID) {
        return -1;
    }
    char replyMessage[5];
    char sendMessage[] = "t";
    int replySize = Send(tid, sendMessage, 2, replyMessage, 5);
    if(replySize == 5 && replyMessage[0] == 'R') {
        int ticks;
        memcpy(&ticks, replyMessage+1, sizeof(ticks));
        // bwprintf(COM2, "Syscall Time - Got number of ticks: %d\n\r", ticks);
        return ticks;
    } else {
        bwprintf(COM2, "Syscall Time - Got invalid value from clock server: %c\n\r", replyMessage[0]);
        return -1;
    }
}

int Delay(int tid, int ticks) {
    if (tid != Constants::CLOCK_SERVER_TID) {
        return -1;
    } 
    if (ticks < 0) {
        return -2;
    }
    char replyMessage[5];
    char sendMessage[5] = "d";

    memcpy(sendMessage+1, &ticks, sizeof(ticks));

    int replySize = Send(tid, sendMessage, 5, replyMessage, 5);

    if(replySize == 5 && replyMessage[0] == 'R') {
        int ticksElapsed;
        memcpy(&ticksElapsed, replyMessage+1, sizeof(ticksElapsed));
        // bwprintf(COM2, "Syscall Delay - Got number of ticks: %d\n\r", ticksElapsed);
        return ticksElapsed;
    } else {
        bwprintf(COM2, "Syscall Delay - Got invalid value from clock server: %c\n\r", replyMessage[0]);
        return -1;
    }
}

int DelayUntil(int tid, int ticks) {
    if (tid != Constants::CLOCK_SERVER_TID) {
        return -1;
    } 
    if (ticks < 0) {
        return -2;
    }

    char replyMessage[5];
    char sendMessage[5] = "u";

    memcpy(sendMessage+1, &ticks, sizeof(ticks));

    int replySize = Send(tid, sendMessage, 5, replyMessage, 5);

    if(replySize == 5 && replyMessage[0] == 'R') {
        int ticksElapsed;
        memcpy(&ticksElapsed, replyMessage+1, sizeof(ticksElapsed));
        // bwprintf(COM2, "Syscall Delay Until - Got number of ticks: %d\n\r", ticksElapsed);
        return ticksElapsed;
    } else {
        bwprintf(COM2, "Syscall Delay Until - Got invalid value from clock server: %c\n\r", replyMessage[0]);
        return -1;
    }
}
