#include <cstring>
#include "../../include/Kernel.hpp"
#include "../../include/UserSyscall.hpp"
#include "../../include/bwio.h"
#include "../../include/ts7200.h"
#include "../../include/Util.hpp"
#include "../../include/Message.hpp"

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

int firstTask() {
    int tid;
    tid = Create(3, testTask);
    bwprintf(COM2, "FirstUserTask: Created Task: %d\n\r", tid);
    tid = Create(3, testTask);
    bwprintf(COM2, "FirstUserTask: Created Task: %d\n\r", tid);
    tid = Create(1, testTask);
    bwprintf(COM2, "FirstUserTask: Created Task: %d\n\r", tid);
    tid = Create(1, testTask);
    bwprintf(COM2, "FirstUserTask: Created Task: %d\n\r", tid);
    bwprintf(COM2, "FirstUserTask: exiting\n\r");
    Exit();
}

void nameServer() {

}

void rpsServer() {
    // TODO: evaluate the size, put it in constants file as well
    DataStructures::RingBuffer<int, 10> registrationList;

    int* sendProcessTid;
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
        responseSize = Receive(sendProcessTid, msg, msglen);

        // process the value of what we received
        if (responseSize != 1) {
            bwprintf(COM2, "Response Size while receiving in RPS is not 1: %d", responseSize);
            Util::assert(false);
        }
  
        if (msg[0] == 's') { //SIGN UP       
            registrationList.push(sendProcessTid);

            if (registrationList.size() >= 2 && gameOver) {
                tidPlayer1 = *registrationList.pop();
                tidPlayer2 = *registrationList.pop();

                gameOver = false;

                Reply(tidPlayer1, ready, 1);
                Reply(tidPlayer2, ready, 1);
            }

        } else if (msg[0] == 'q') { //QUIT

            // TODO: what to do if both send q at same time???

            // Let the client go
            Reply(*sendProcessTid, quit, 1);

            // Set gameover state to be true so that on next iteration other task can be let go
            gameOver = true;

        } else if (msg[0] == 'r' || msg [0] == 'p' || msg[0] == 'x') { // PLAY

            // If one client has already called quits to the game
            if (gameOver) {
                // send quit signal to the other process that the game has ended
                Reply(*sendProcessTid, quit, 1);

                // handle popping off next elements which are ready and then get out of this loop
                if (registrationList.size() >= 2) {
                    // release the next 2 if available
                    tidPlayer1 = *registrationList.pop();
                    tidPlayer2 = *registrationList.pop();

                    gameOver = false;

                    Reply(tidPlayer1, ready, 1);
                    Reply(tidPlayer2, ready, 1);   
                    continue;
                }
            }

            if (*sendProcessTid == tidPlayer1) {
                responsePlayer1 = msg[0];
                hasPlayer1Responded = true;
            } else if (*sendProcessTid == tidPlayer2) {
                responsePlayer2 = msg[0];
                hasPlayer2Responded = true;
            } else {
                bwprintf(COM2, "A task which shouldnt have sent message, sent a message!");
            }

            if (hasPlayer1Responded && hasPlayer2Responded) {  
                if (responsePlayer1 == responsePlayer2) { 
                    Reply(tidPlayer1, draw, 1);
                    Reply(tidPlayer2, draw, 1);

                } else if ((responsePlayer1 == 'p' && responsePlayer2 = 's') ||
                (responsePlayer1 == 'x' && responsePlayer2 = 'p') ||
                (responsePlayer1 == 's' && responsePlayer2 = 'x')) {  
                    Reply(tidPlayer1, win, 1);
                    Reply(tidPlayer2, loss, 1);
                } else {

                    Reply(tidPlayer1, loss, 1);
                    Reply(tidPlayer2, win, 1);

                }
                responsePlayer1 = responsePlayer2 = '\0';
                hasPlayer1Responded = false;
                hasPlayer2Responded = false;             
            }            
        } else { // INVALID

            bwprintf(COM2, "Got garbage message from the Client task: %c", msg[0]);
            Util::assert(false);

        }

    }   
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
    handleCreate(2, firstTask);
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
            handleSend((SendRequest *) arg1);
            break;

        case 8:
            handleReceive((int *) arg1, (int *) arg2, (int) arg3);
            break;

        case 9:
            handleReply((int) arg1, (const char *)arg2, (int) arg3);
            break;

        default:
            break;
    }

    switch (activeTask->taskState) {
        case Constants::READY:
            ready_queue.push(activeTask, activeTask->priority);
            break;

        case Constants::ZOMBIE:
            exit_queue.push(activeTask);
            break;

        case Constants::ACTIVE:
            bwprintf(COM2, "Task shouldn't have ACTIVE state\n");
            break;

        case Constants::SEND_BLOCKED:
        case Constants::RECEIVE_BLOCKED:
        case Constants::REPLY_BLOCKED:
            break;

        default:
            bwprintf(COM2, "Got invalid task State: %d \n", activeTask->taskState);
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
    if (receiver == nullptr) {
        // HANDLE ERROR CASE
    }
    activeTask->kSendRequest.receiverTD = receiver;
    activeTask->kSendRequest.sendRequest = sendRequest;
    if (receiver->taskState == Constants::RECEIVE_BLOCKED) {
        // Transition the receiver to be ready and puts them on the ready queue
        ready_queue.push(receiver, receiver->priority);
        receiver->taskState = Constants::READY;
        receiver->receiveQueue.push(&activeTask->kSendRequest);
        // Transition the sender to be reply blocked
        activeTask->taskState = Constants::REPLY_BLOCKED;
    } else {
        // Transition the sender to be send blocked
        activeTask->taskState = Constants::SEND_BLOCKED;
    }
}

int Kernel::handleReceive(int *tid, int *msg, int msglen) {
    if (!activeTask->receiveQueue.empty()) {
        KernelSendRequest *kSendRequest = activeTask->receiveQueue.pop();
        // TODO: compare msglen and ksr messagelen and do appropriate action
        memcpy(msg, kSendRequest->sendRequest->msg, kSendRequest->sendRequest->msglen);
        *tid = kSendRequest->senderTD->tid;
        // Transition sender to be reply blocked now
        replyQueue.push(kSendRequest);
        kSendRequest->senderTD->taskState = Constants::REPLY_BLOCKED;
    } else {
        activeTask->taskState = Constants::RECEIVE_BLOCKED;
    }
}

int Kernel::handleReply(int tid, const char *reply, int rplen) {
    KernelSendRequest *kSendRequest;
    for (int i = 0; i < replyQueue.size(); ++i) {
        kSendRequest = replyQueue.pop();
        if (kSendRequest->senderTD->tid == tid) {
            // TODO: Compare lengths
            memcpy(kSendRequest->sendRequest->reply, reply, rplen);
            ready_queue.push(kSendRequest->senderTD, kSendRequest->senderTD->priority);
            kSendRequest->senderTD->taskState = Constants::READY;
        } else {
            replyQueue.push(kSendRequest);
        }
    }
    // TODO: How do we check for this?
    return -2; // tid is not the tid of an existing task
    return -3; // tid is not the tid of a reply-blocked task
    // check if tid is reply blocked
    // check if tid is an existing task
    // if message is truncated return -1 but copy what you can
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
