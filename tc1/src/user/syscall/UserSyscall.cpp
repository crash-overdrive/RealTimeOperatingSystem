#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "kern/Message.hpp"
#include "user/message/TRMessage.hpp"
#include "user/message/ThinMessage.hpp"
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

int RegisterAs(const char* name) {
    char replyMessage[Constants::NameServer::REPLY_MESSAGE_MAX_SIZE];

    int sendMessageSize = strlen(name) + 1;
    char sendMessage[sendMessageSize];
    memcpy(sendMessage, name, sendMessageSize);
    sendMessage[sendMessageSize - 1] = Constants::NameServer::REGISTER_AS;

    int replyMessageSize = Send(Constants::NameServer::TID, sendMessage, sendMessageSize, replyMessage, Constants::NameServer::REPLY_MESSAGE_MAX_SIZE);

    if (!strcmp(replyMessage, Constants::NameServer::SUCCESS_REPLY) && replyMessageSize > 0) {
        return 0;
    }
    bwprintf(COM2, "RegisterAs Failure: %d\n\r", name);
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
    bwprintf(COM2, "WhoIs Failure: %s\n\r", name);
    return -1;
}

int Time(int tid) {
    if (tid != Constants::ClockServer::TID) {
        return -1;
    }

    char replyMessage[Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE];

    char sendMessage[1];
    sendMessage[0] = Constants::ClockServer::TIME;

    int replySize = Send(tid, sendMessage, 1, replyMessage, Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE);

    if(replySize == 5 && replyMessage[0] == Constants::ClockServer::ACKNOWLEDGE) {
        int ticks;
        memcpy(&ticks, replyMessage+1, sizeof(ticks));
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

    char replyMessage[Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE];

    char sendMessage[Constants::ClockServer::SEND_MESSAGE_MAX_SIZE];
    sendMessage[0] = Constants::ClockServer::DELAY;
    memcpy(sendMessage+1, &ticks, sizeof(ticks));

    int replySize = Send(tid, sendMessage, 5, replyMessage, Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE);

    if(replySize == 5 && replyMessage[0] == Constants::ClockServer::ACKNOWLEDGE) {
        int ticksElapsed;
        memcpy(&ticksElapsed, replyMessage+1, sizeof(ticksElapsed));
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

    char replyMessage[Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE];

    char sendMessage[Constants::ClockServer::SEND_MESSAGE_MAX_SIZE];
    sendMessage[0] = Constants::ClockServer::DELAY_UNTIL;
    memcpy(sendMessage+1, &ticks, sizeof(ticks));

    int replySize = Send(tid, sendMessage, 5, replyMessage, Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE);

    if(replySize == 5 && replyMessage[0] == Constants::ClockServer::ACKNOWLEDGE) {
        int ticksElapsed;
        memcpy(&ticksElapsed, replyMessage+1, sizeof(ticksElapsed));
        return ticksElapsed;
    } else {
        bwprintf(COM2, "Syscall Delay Until - Got invalid value from clock server: %c\n\r", replyMessage[0]);
        return -1;
    }
}

int Getc(int tid, int uart) {
    char c;
    int rplen;

    // If we don't get a valid uart or the tid doesn't match a valid server, return error
    if ((uart != UART1 && uart != UART2) ||
        (uart == UART1 && tid != Constants::UART1RXServer::TID) ||
        (uart == UART2 && tid != Constants::UART2RXServer::TID)) {
        return -1;
    }

    rplen = Send(tid, "", 1, &c, 1);
    if (rplen != 1) {
        bwprintf(COM2, "Syscall - Getc reply length not 1");
    }

    return c;
}

int Putc(int tid, int uart, char ch) {
    int rplen;
    char reply;

    // If we don't get a valid uart or the tid doesn't match a valid server, return error
    if ((uart != UART1 && uart != UART2) ||
        (uart == UART1 && tid != Constants::UART1TXServer::TID) ||
        (uart == UART2 && tid != Constants::UART2TXServer::TID)) {
        return -1;
    }

    rplen = Send(tid, &ch, 1, &reply, 1);

    if (rplen != 1) {
        bwprintf(COM2, "Syscall - Putc reply length not 1");
    }

    return 0;
}

void Shutdown() {
    int MARKLIN = WhoIs("MARKLIN");
    int CLOCK = WhoIs("CLOCK");
    TRMessage trmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);
    trmsg.train = 1;
    Send(MARKLIN, (char*)&trmsg, trmsg.size(), (char*)&rdymsg, rdymsg.size());
    trmsg.train = 24;
    Send(MARKLIN, (char*)&trmsg, trmsg.size(), (char*)&rdymsg, rdymsg.size());
    trmsg.train = 58;
    Send(MARKLIN, (char*)&trmsg, trmsg.size(), (char*)&rdymsg, rdymsg.size());
    trmsg.train = 74;
    Send(MARKLIN, (char*)&trmsg, trmsg.size(), (char*)&rdymsg, rdymsg.size());
    trmsg.train = 78;
    Send(MARKLIN, (char*)&trmsg, trmsg.size(), (char*)&rdymsg, rdymsg.size());
    rdymsg.mh.type = Constants::MSG::STOP;
    Send(MARKLIN, (char*)&trmsg, trmsg.size(), (char*)&rdymsg, rdymsg.size());
    Delay(CLOCK, 100);
    sysShutdown();
}

int Halt() {
    int idleTime = sysHalt();
    return idleTime;
}
