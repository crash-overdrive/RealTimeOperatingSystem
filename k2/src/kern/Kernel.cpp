#include <cstring>
#include "../../include/Kernel.hpp"
#include "../../include/UserSyscall.hpp"
#include "../../include/bwio.h"
#include "../../include/ts7200.h"
#include "../../include/Util.hpp"
#include "../../include/Message.hpp"
#include "../../include/TimingTasks.hpp"

#define print(str) bwprintf(COM2, str)

volatile int reg0;
volatile int reg1;
volatile int reg2;
volatile int reg3;
volatile int reg4;
volatile int reg5;
volatile int reg6;
volatile int reg7;

void printRegisters(int* stack){
    bwprintf(COM2, "Stack pointer : %x\r\n", stack);
    int i=0;
    for(i=0;i<16;i++){
        bwprintf(COM2, "%x at %x\r\n", stack[i], stack+i);
    }
}

void testTask() {
    int tid, ptid;
    tid = MyTid();
    ptid = MyParentTid();
    bwprintf(COM2, "TestTask: My tid is %d and my parent's tid is %d\n\r", tid, ptid);
    Yield();
    bwprintf(COM2, "TestTask: My tid is %d and my parent's tid is %d\n\r", tid, ptid);
    Exit();
}

void rpsClientROCK() {
    char* signup = "s";
    char* quit = "q";
    char* rock = "r";
    char response[5];

    int rpsServer = 3;

    int reply = Send(rpsServer, signup, 2, response, 5);

    if (reply == 2 && response[0] == 'r') {
        for (int i = 1; i <= 3; ++i) {
            reply = Send(rpsServer, rock, 2, response, 5);
            if (reply == 2) {
                if (response[0] == 'w') {
                    bwprintf(COM2, "Rock client won\n\r");
                } else if (response[0] == 'l') {
                    bwprintf(COM2, "Rock client lost\n\r");
                } else if (response[0] == 'd') {
                    bwprintf(COM2, "Rock client draw\n\r");
                } else if (response[0] == 'q') {
                    bwprintf(COM2, "Other player quit..\n\r");
                    break;
                } else {
                    bwprintf(COM2, "RPS CLIENT ROCK Received bad value: %s\n\r", response);
                }

            } else {
                bwprintf(COM2, "RPS CLIENT ROCK Received incorrect length: %d - bad value: %s\n\r", reply, response);
            }
        }
        reply = Send(rpsServer, quit, 2, response, 5);
    } else {
        bwprintf(COM2, "RPS CLIENT ROCK Received incorrect length: %d - bad value: %s\n\r", reply, response);
    }
    bwprintf(COM2, "Rock Client exiting!!\n\r");
    Exit();

}

void rpsClientPAPER() {
    char* signup = "s";
    char* quit = "q";
    char* paper = "p";
    char response[5];
    int nameServer = 2;

    int rpsServer = 3;

    int reply = Send(rpsServer, signup, 2, response, 5);

    if (reply == 2 && response[0] == 'r') {
        for (int i = 1; i <= 3; ++i) {
            reply = Send(rpsServer, paper, 2, response, 5);
            if (reply == 2) {
                if (response[0] == 'w') {
                    bwprintf(COM2, "Paper client won\n\r");
                } else if (response[0] == 'l') {
                    bwprintf(COM2, "Paper client lost\n\r");
                } else if (response[0] == 'd') {
                    bwprintf(COM2, "Paper client draw\n\r");
                } else if (response[0] == 'q') {
                    bwprintf(COM2, "Other player quit..\n\r");
                    break;
                } else {
                    bwprintf(COM2, "RPS CLIENT Paper Received bad value: %s\n\r", response);
                }

            } else {
                bwprintf(COM2, "RPS CLIENT Paper Received incorrect length: %d  - bad value: %s\n\r", reply, response);
            }
        }
        reply = Send(rpsServer, quit, 2, response, 5);
    } else {
        bwprintf(COM2, "RPS CLIENT Paper Received incorrect length - bad value: %s\n\r", reply, response);
    }
    bwprintf(COM2, "Paper Client exiting!!\n\r");
    Exit();

}

void rpsClientSCISSOR() {
    char* signup = "s";
    char* quit = "q";
    char* scissor = "x";
    char response[5];

    int rpsServer = 3;

    int reply = Send(rpsServer, signup, 2, response, 5);

    if (reply == 2 && response[0] == 'r') {
        for (int i = 1; i <= 3; ++i) {
            reply = Send(rpsServer, scissor, 2, response, 5);
            if (reply == 2) {
                if (response[0] == 'w') {
                    bwprintf(COM2, "Scissor client won\n\r");
                } else if (response[0] == 'l') {
                    bwprintf(COM2, "Scissor client lost\n\r");
                } else if (response[0] == 'd') {
                    bwprintf(COM2, "Scissor client draw\n\r");
                } else if (response[0] == 'q') {
                    bwprintf(COM2, "Other player quit..\n\r");
                    break;
                } else {
                    bwprintf(COM2, "RPS CLIENT Scissor Received bad value: %s\n\r", response);
                }

            } else {
                bwprintf(COM2, "RPS CLIENT Scissor Received incorrect length: %d - bad value: %s\n\r", reply, response);
            }
        }
        reply = Send(rpsServer, quit, 2, response, 5);
    } else {
        bwprintf(COM2, "RPS CLIENT Scissor Received incorrect length: %d - bad value: %s\n\r", reply, response);
    }
    bwprintf(COM2, "Scissor Client exiting!!\n\r");
    Exit();

}


void nameServer() {
    Exit();
    int sendProcessTid = -1;
    // TODO: evaluate the size, put it in constants file as well
    int namelen = 33; 
    char name[namelen];
    int responseSize;
    FOREVER {
        responseSize = Receive(&sendProcessTid, name, namelen);

        if (responseSize > 0) {

        }
    }
}

void rpsServer() {
    // TODO: evaluate the size, put it in constants file as well
    DataStructures::RingBuffer<int, 10> registrationList;

    int sendProcessTid = -1;
    // TODO: evaluate the size, put it in constants file as well
    int msglen = 5; 
    char msg[msglen];
    int responseSize;

    int tidPlayer1 = -1;
    int tidPlayer2 = -1;
    char responsePlayer1 = '\0';
    char responsePlayer2 = '\0';
    bool hasPlayer1Responded = false;
    bool hasPlayer2Responded = false;
    char* ready = "r";
    char* quit = "q";
    char* win = "w";
    char* loss = "l";
    char* draw = "d";
    bool gameOver = true;

    FOREVER {
        
        responseSize = Receive(&sendProcessTid, msg, msglen);

        // process the value of what we received
        if (responseSize != 2) {
            bwprintf(COM2, "Response Size while receiving in RPS is not 2: %d", responseSize);
            Util::assert(false);
        }
  
        if (msg[0] == 's') { //SIGN UP       
            registrationList.push((int *)sendProcessTid);

            if (registrationList.size() >= 2 && gameOver) {
                tidPlayer1 = (int)registrationList.pop();
                tidPlayer2 = (int)registrationList.pop();
            
                gameOver = false;

                Reply(tidPlayer1, ready, 2);
                Reply(tidPlayer2, ready, 2);
            }

        } else if (msg[0] == 'q') { //QUIT

            // Let the client go
            Reply(sendProcessTid, quit, 2);

            // both clients send q on same iteration
            if (gameOver && registrationList.size() >= 2) {
                tidPlayer1 = (int)registrationList.pop();
                tidPlayer2 = (int)registrationList.pop();

                gameOver = false;

                Reply(tidPlayer1, ready, 2);
                Reply(tidPlayer2, ready, 2);
                
                continue;
            }

            // Set gameover state to be true so that on next iteration other task can be let go
            gameOver = true;

        } else if (msg[0] == 'r' || msg [0] == 'p' || msg[0] == 'x') { // PLAY

            // If one client has already called quits to the game
            if (gameOver) {
                // send quit signal to the other process that the game has ended
                Reply(sendProcessTid, quit, 2);

                // handle popping off next elements which are ready and then get out of this loop
                if (registrationList.size() >= 2) {
                    // release the next 2 if available
                    tidPlayer1 = *registrationList.pop();
                    tidPlayer2 = *registrationList.pop();

                    gameOver = false;

                    Reply(tidPlayer1, ready, 2);
                    Reply(tidPlayer2, ready, 2);   
                    continue;
                }
            }

            if (sendProcessTid == tidPlayer1) {
                responsePlayer1 = msg[0];
                hasPlayer1Responded = true;
            } else if (sendProcessTid == tidPlayer2) {
                responsePlayer2 = msg[0];
                hasPlayer2Responded = true;
            } else {
                bwprintf(COM2, "A task which shouldnt have sent message, sent a message!");
            }

            if (hasPlayer1Responded && hasPlayer2Responded) {  
                bwprintf(COM2, "Press any key to continue...\n\r");
                bwgetc(COM2);
                if (responsePlayer1 == responsePlayer2) { 
                    Reply(tidPlayer1, draw, 2);
                    Reply(tidPlayer2, draw, 2);

                } else if ((responsePlayer1 == 'p' && responsePlayer2 == 'r') ||
                (responsePlayer1 == 'x' && responsePlayer2 == 'p') ||
                (responsePlayer1 == 'r' && responsePlayer2 == 'x')) {  
                    Reply(tidPlayer1, win, 2);
                    Reply(tidPlayer2, loss, 2);

                } else {
                    Reply(tidPlayer1, loss, 2);
                    Reply(tidPlayer2, win, 2);

                }
                responsePlayer1 = responsePlayer2 = '\0';
                hasPlayer1Responded = false;
                hasPlayer2Responded = false;
            }
        }
    }
}

int firstTask() {
    int tid;
    tid = Create(2, nameServer);
    tid = Create(2, rpsServer);
    tid = Create(1, rpsClientPAPER);
    tid = Create(1, rpsClientROCK);
    tid = Create(1, rpsClientROCK);
    tid = Create(1, rpsClientSCISSOR);
    tid = Create(1, rpsClientPAPER);
    tid = Create(1, rpsClientSCISSOR);
    // tid = Create(1, sendTask);
    // tid = Create(1, receiveTask);

    Exit();
}

void Kernel::initialize() {
    // Setup comm
    uart.setConfig(COM1, BPF8, OFF, ON, OFF);
	uart.setConfig(COM2, BPF8, OFF, OFF, OFF);

    // Setup TaskDescriptors
    for (int i = 0; i < Constants::NUM_TASKS; ++i) {
        // Set all task ids to -1 so lookup can be performed safely
        tasks[i].tid = -1;
        tasks[i].kSendRequest.senderTD = &tasks[i];
    }

    // Setup 0x8, 0x28
    // asm volatile("mov r12, #0xe59ff018"); // e59ff018 = ldr pc, [pc, #24]
    // asm volatile("str r12, #0x8");
    *(int*)0x8 = 0xe59ff018; // e59ff018 = ldr pc, [pc, #24]
    asm volatile("ldr r12, =context_switch_entry");
    asm volatile("ldr r3, =0x28");
    asm volatile("str r12, [r3]");

    // Create the system's first task
    handleCreate(0, firstTask);
}

void Kernel::schedule() {
    // TODO: what happens when ready_queue is empty?
    activeTask = ready_queue.pop();
}

int Kernel::activate() {
    reg0 = activeTask->cpsr;
    reg1 = (int)activeTask->pc;
    reg2 = activeTask->r0;
    reg3 = (int)activeTask->sp;

    asm volatile("ldr pc, =context_switch_exit");



    // Should never reach here!! some bug if it reached here!
    bwprintf(COM2, "If you see this then something is really wrong\n");
    Util::assert(false);



    asm volatile(".globl kernel_entry");
    asm volatile("kernel_entry:");
    arg1 = (void *) reg0;
    arg2 = (void *) reg1;
    arg3 = (void *) reg2;
    arg4 = (void *) reg3;
    activeTask->sp = (int *) reg5;
    activeTask->pc = reg7;

    return reg4;
    
}

void Kernel::handle(int request)  {
    // Set the state of the activeTask to be READY
    // If it needs to be changed then the appropriate handler will do it
    activeTask->taskState = Constants::READY;

    switch(request) {
        int kernelRequestResponse;
        case 2:
            // TODO: refactor this to eliminate intermediate value
            kernelRequestResponse = handleCreate((int)arg1, (int (*)())arg2);
            activeTask->r0 = kernelRequestResponse;
            break;

        case 3:
            kernelRequestResponse = handleMyTid();
            activeTask->r0 = kernelRequestResponse;
            break;

        case 4:
            kernelRequestResponse = handleMyParentTid();
            activeTask->r0 = kernelRequestResponse;
            break;

        case 5:
            break;

        case 6:
            handleExit();
            break;

        case 7:
            // bwprintf(COM2, "Active Task Tid before handleSend: %d\n\r", activeTask->tid);
            activeTask->r0 = handleSend((SendRequest *) arg1);
            // bwprintf(COM2, "Active Task Tid after handleSend: %d\n\r", activeTask->tid);
            // bwprintf(COM2, "Came out from handlesend\n\r");
            break;

        case 8:
            activeTask->r0 = handleReceive((int *) arg1, (int *) arg2, (int) arg3);
            // bwprintf(COM2, "Came out from handleReceive\n\r");
            break;

        case 9:
            activeTask->r0 = handleReply((int) arg1, (const char *)arg2, (int) arg3);
            // bwprintf(COM2, "Came out from handleReply\n\r");
            break;

        default:
            break;
    }

    switch (activeTask->taskState) {
        case Constants::READY:
            // bwprintf(COM2, "Rescheduled task tid: %d\n\r", activeTask->tid);
            ready_queue.push(activeTask, activeTask->priority);
            break;

        case Constants::ZOMBIE:
            exit_queue.push(activeTask);
            break;

        case Constants::ACTIVE:
            bwprintf(COM2, "Task shouldn't have ACTIVE state\n\r");
            break;

        case Constants::SEND_BLOCKED:
            // bwprintf(COM2, "TID: %d on SEND_BLOCKED\n\r", activeTask->tid);
            break;
        case Constants::RECEIVE_BLOCKED:
            // bwprintf(COM2, "TID: %d on RECEIVE_BLOCKED\n\r", activeTask->tid);
            break;
        case Constants::REPLY_BLOCKED:
            // bwprintf(COM2, "TID: %d on REPLY_BLOCKED\n\r", activeTask->tid);
            break;

        default:
            bwprintf(COM2, "Received invalid task State: %d \n\r", activeTask->taskState);
            break;
    }
    
}

int Kernel::handleCreate(int priority, int (*function)()) {
    taskNumber++;
    availableTid++;

    if (priority < 0 || priority >= Constants::NUM_PRIORITIES) {
        return -1;
    }

    if (taskNumber >= Constants::NUM_TASKS) {
        return -2;  
    }
    
    TaskDescriptor* newTD = &tasks[taskNumber];

    newTD->tid = availableTid;
    // bwprintf(COM2, "Created task with TID: %d\n\r", availableTid);

    if (activeTask == nullptr) {
        newTD->parentTid = 0;
    } else {
        newTD->parentTid = activeTask->tid;
    }
    
    newTD->priority = priority;
    newTD->taskState = Constants::READY;
    newTD->r0 = 0;
    newTD->cpsr = 0b10000;

    // TODO: check validity of pc
    // TODO: wrap function in another function with exit()
    newTD->pc = (int)function;

    // set the stack to dummy values [r4-r11, lr]
    newTD->stack[32767] = 0xdeadbeef; // for debugging purposes
    newTD->stack[32766] = 1; // r4
    newTD->stack[32765] = 2; // r5
    newTD->stack[32764] = 3; // r6
    newTD->stack[32763] = 4; // r7
    newTD->stack[32762] = 5; // r8
    newTD->stack[32761] = 6; // r9
    newTD->stack[32760] = 7; // r10
    newTD->stack[32759] = 8; // r11
    newTD->stack[32758] = 9; // TODO: fix lrs values (? what is this?)

    newTD->sp = &(newTD->stack[32758]);

    ready_queue.push(newTD, newTD->priority);


    return availableTid;
}

int Kernel::handleMyTid() {
    int test = activeTask->tid;
    return test;
    // return activeTask->tid;
}

int Kernel::handleMyParentTid() {
    int test = activeTask->parentTid;
    return test;
    // return activeTask->parentTid;
}

void Kernel::handleExit() {
    activeTask->taskState = Constants::ZOMBIE;
}

int Kernel::handleSend(SendRequest *sendRequest) {
    TaskDescriptor *receiver = lookupTD(sendRequest->tid);

    // Check if the task exists
    if (receiver == nullptr || receiver->taskState == Constants::ZOMBIE) {
        return -1; // Task id did not correspond to a living task
    }

    activeTask->kSendRequest.receiverTD = receiver;
    activeTask->kSendRequest.sendRequest = sendRequest;

    receiver->receiveQueue.push(&activeTask->kSendRequest);

    if (receiver->taskState == Constants::RECEIVE_BLOCKED) {
        // Receiver is already ready for message to be copied
        *(receiver->kReceiveRequest.tid) = activeTask->tid;
        if (receiver->kReceiveRequest.msglen <= sendRequest->msglen) {
            memcpy(receiver->kReceiveRequest.msg, sendRequest->msg, receiver->kReceiveRequest.msglen);
            receiver->r0 = receiver->kReceiveRequest.msglen;
        } else {
            memcpy(receiver->kReceiveRequest.msg, sendRequest->msg, sendRequest->msglen);
            receiver->r0 = sendRequest->msglen;
        }

        // Transition the receiver to be ready and puts them on the ready queue
        ready_queue.push(receiver, receiver->priority);
        receiver->taskState = Constants::READY;
        // Transition the sender to be reply blocked
        replyQueue.push(&activeTask->kSendRequest);
        activeTask->taskState = Constants::REPLY_BLOCKED;

        return -3; // the return value will be set by handleReply
    } else {
        // Transition the sender to be send blocked
        activeTask->taskState = Constants::SEND_BLOCKED;

        return -4; // In this case the return value will be set by handleReply
    }
}

int Kernel::handleReceive(int *tid, int *msg, int msglen) {
    // bwprintf(COM2, "Size of receive: %d\n\r", activeTask->receiveQueue.size());
    if (!activeTask->receiveQueue.empty()) {
        KernelSendRequest *kSendRequest = activeTask->receiveQueue.pop();

        if (msglen <= kSendRequest->sendRequest->msglen) {
            memcpy(msg, kSendRequest->sendRequest->msg, msglen);
        } else {
            memcpy(msg, kSendRequest->sendRequest->msg, kSendRequest->sendRequest->msglen);
        }

        *tid = kSendRequest->senderTD->tid;
        // Transition sender to be reply blocked
        replyQueue.push(kSendRequest);
        kSendRequest->senderTD->taskState = Constants::REPLY_BLOCKED;

        return msglen <= kSendRequest->sendRequest->msglen ? msglen : kSendRequest->sendRequest->msglen; // return the lesser of the two message lengths
    } else {
        // Setup kernel receive request for later retrieval
        activeTask->kReceiveRequest.tid = tid;
        activeTask->kReceiveRequest.msg = msg;
        activeTask->kReceiveRequest.msglen = msglen;

        // Transition receiver to be receive blocked
        activeTask->taskState = Constants::RECEIVE_BLOCKED;

        return -1;
    }
}

int Kernel::handleReply(int tid, const char *reply, int rplen) {
    KernelSendRequest *kSendRequest;

    // Check if the task is reply-blocked
    for (int i = 0; i < replyQueue.size(); ++i) {
        kSendRequest = replyQueue.pop();
        if (kSendRequest->senderTD->tid == tid) {
            // if reply is truncated return -1 but copy what you can
            if (rplen > kSendRequest->sendRequest->rplen) {
                memcpy(kSendRequest->sendRequest->reply, reply, kSendRequest->sendRequest->rplen);
                kSendRequest->senderTD->r0 = kSendRequest->sendRequest->rplen;
                kSendRequest->senderTD->taskState = Constants::READY;
                ready_queue.push(kSendRequest->senderTD, kSendRequest->senderTD->priority);

                return -1; // reply was truncated
            } else {
                memcpy(kSendRequest->sendRequest->reply, reply, rplen);
                kSendRequest->senderTD->r0 = rplen; // Reply length was shorter so save that information
                kSendRequest->senderTD->taskState = Constants::READY;
                ready_queue.push(kSendRequest->senderTD, kSendRequest->senderTD->priority);

                return 0; // successful reply
            }
        } else {
            replyQueue.push(kSendRequest);
        }
    }

    // Check if the task exists
    for (int i = 0; i < Constants::NUM_TASKS; ++i) {
        if (tasks[i].tid == tid && tasks[i].taskState != Constants::REPLY_BLOCKED && tasks[i].taskState != Constants::ZOMBIE) {
            return -3; // tid is not the tid of a reply-blocked task
        }
    }

    return -2; // tid is not the tid of an existing task
}

TaskDescriptor *Kernel::lookupTD(int tid) {
    for (int i = 0; i < Constants::NUM_TASKS; ++i) {
        if (tasks[i].tid == tid) {
            if (tasks[i].taskState == Constants::ZOMBIE) { return nullptr; }
            return &tasks[i];
        }
    }
    return nullptr;
}

void Kernel::run() {
    initialize();
    FOREVER {
        schedule();
        if (activeTask == nullptr) { bwprintf(COM2, "No active tasks scheduled!"); break; }
        request = activate();
        handle(request);
    }
}
