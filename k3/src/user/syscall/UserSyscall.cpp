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

int AwaitEvent(int eventId) {
    int retval;
    asm volatile("swi %c0" :: "i"(Constants::SWI::AWAIT_EVENT));
    asm volatile("mov %0, r0" : "=r"(retval));
    return retval;
}

//TODO: find a way to encode the request
int RegisterAs(const char* name) {
    char replyMessage[Constants::NameServer::REPLY_MESSAGE_MAX_SIZE];

    int sendMessageSize = strlen(name) + 1;
    char sendMessage[sendMessageSize];
    memcpy(sendMessage, name, sendMessageSize);
    sendMessage[sendMessageSize - 1] = Constants::NameServer::REGISTER_AS;

    int replyMessageSize = Send(Constants::NameServer::TID, sendMessage, sendMessageSize, replyMessage, Constants::NameServer::REPLY_MESSAGE_MAX_SIZE);
    
    if (replyMessageSize == 1 && replyMessage[0] == Constants::NameServer::SUCCESS_REPLY) {
                    
        return 0;
        
    }
    return -1;
}

int WhoIs(const char* name) {
    char replyMessage[Constants::NameServer::REPLY_MESSAGE_MAX_SIZE];

    int sendMessageSize = strlen(name) + 1;
    char sendMessage[sendMessageSize];
    memcpy(sendMessage, name, sendMessageSize);
    sendMessage[sendMessageSize - 1] = Constants::NameServer::WHO_IS;


    int replyMessageSize = Send(Constants::NameServer::TID, sendMessage, sendMessageSize, replyMessage, Constants::NameServer::REPLY_MESSAGE_MAX_SIZE);
    
    if (replyMessageSize == sizeof(int)) {
        int tid;
        memcpy(&tid, replyMessage, replyMessageSize);
        return tid;

    }
    return -1;
}

int Time(int tid) {
    if (tid != Constants::ClockServer::TID) {
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
    if (tid != Constants::ClockServer::TID) {
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
    if (tid != Constants::ClockServer::TID) {
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
