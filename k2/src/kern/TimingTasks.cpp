#include "../../include/TimingTasks.hpp"
#include "../../include/Util.hpp"
#include "../../include/UserSyscall.hpp"
#include "../../include/bwio.h"

#define TIMER_START_VALUE 0xFFFFFFFF

#define READ_REGISTER * (int *)
#define WRITE_REGISTER * (int *)

#define TIMER3_LOAD 0x80810080
#define TIMER3_VALUE 0x80810084
#define TIMER3_CONTROL 0x80810088

void sendTask() {
    // int receiveServer = WhoIs("ReceiveServer");
    char msg[256] = "HeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHe\n";
    char reply[256];
    int replySize = 4;
    // replySize = Send(receiveServer, msg, 14, &reply, 32);
    for (int i = 0; i < 100000; ++i) {
        replySize = Send(2, msg, 4, reply, 4);
    }
    
    // bwprintf(COM2, "DEBUG: Size of message returned to SEND is %d with msg %s\n\r", replySize, reply);
    Exit();
}

void receiveTask() {
    WRITE_REGISTER TIMER3_LOAD = 0xFFFFFFFF;
	WRITE_REGISTER TIMER3_CONTROL = 0x88; // set with free running mode at 508 KHz
    

    int tid = 0;
    char msg[256];
    int msglen = 0;
    int repret = 1;
    char reply[256] = "HeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHeHe\n";

    int startTime = READ_REGISTER TIMER3_VALUE;
    
    for (int i = 0; i < 100000; ++i) {
        msglen = Receive(&tid, msg, 4);
        // bwprintf(COM2, "DEBUG: RECEIVED a message <%s> with length %d from tid %d\n\r", msg, msglen, tid);
        repret = Reply(tid, reply, 4);
        // bwprintf(COM2, "DEBUG: Our REPLY returned the code %d and msg %s\n\r", repret, reply);
    }

    int endTime = READ_REGISTER TIMER3_VALUE;
    int timeTaken = (startTime - endTime)/1000000;
    bwprintf(COM2, "Start Time: %d\n\r", startTime);
    bwprintf(COM2, "End Time: %d\n\r", endTime);
    bwprintf(COM2, "Time taken for 1 SRR: %d\n\r", timeTaken);
        
    Exit();
}