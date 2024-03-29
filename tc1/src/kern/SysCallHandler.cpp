#include <cstring>
#include "kern/Kernel.hpp"
#include "kern/TaskDescriptor.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"

int Kernel::handleCreate(int priority, void (*function)()) {
    taskNumber++;

    if (priority < 0 || priority >= Constants::NUM_PRIORITIES) {
        return -1;
    }

    if (taskNumber >= Constants::NUM_TASKS) {
        return -2;
    }

    TaskDescriptor* newTD = &tasks[taskNumber];

    newTD->tid = taskNumber;
    // bwprintf(COM2, "Created task with TID: %d\n\r", taskNumber);

    if (activeTask == nullptr) {
        newTD->parentTid = 0;
    } else {
        newTD->parentTid = activeTask->tid;
    }

    newTD->priority = priority;
    newTD->taskState = Constants::READY;

    // set the stack to dummy values [r0-r12, lr, pc, cpsr]
    newTD->stack[Constants::TD_STACK_SIZE - 1] = 0xdeadbeef; // for debugging purposes
    newTD->stack[Constants::TD_STACK_SIZE - 2] = 13; // lr
    newTD->stack[Constants::TD_STACK_SIZE - 3] = 12; // r12
    newTD->stack[Constants::TD_STACK_SIZE - 4] = 11; // r11
    newTD->stack[Constants::TD_STACK_SIZE - 5] = 10; // r10
    newTD->stack[Constants::TD_STACK_SIZE - 6] = 9; // r9
    newTD->stack[Constants::TD_STACK_SIZE - 7] = 8; // r8
    newTD->stack[Constants::TD_STACK_SIZE - 8] = 7; // r7
    newTD->stack[Constants::TD_STACK_SIZE - 9] = 6; // r6
    newTD->stack[Constants::TD_STACK_SIZE - 10] = 5; // r5
    newTD->stack[Constants::TD_STACK_SIZE - 11] = 4; // r4
    newTD->stack[Constants::TD_STACK_SIZE - 12] = 3; // r3
    newTD->stack[Constants::TD_STACK_SIZE - 13] = 2; // r2
    newTD->stack[Constants::TD_STACK_SIZE - 14] = 1; // r1
    newTD->stack[Constants::TD_STACK_SIZE - 15] = 0; // r0
    newTD->stack[Constants::TD_STACK_SIZE - 16] = 0b01010000; // cpsr
    newTD->stack[Constants::TD_STACK_SIZE - 17] = (int)function; // pc
    newTD->stack[Constants::TD_STACK_SIZE - 18] = 0; // IRQ handle sentinal

    newTD->sp = &(newTD->stack[Constants::TD_STACK_SIZE - 18]);

    ready_queue.push(newTD, newTD->priority);

    return taskNumber;
}

int Kernel::handleMyTid() {
    return activeTask->tid;
}

int Kernel::handleMyParentTid() {
   return activeTask->parentTid;
}

void Kernel::handleExit() {
    activeTask->taskState = Constants::STATE::ZOMBIE;
}

int Kernel::handleSend(SendRequest *sendRequest) {
    TaskDescriptor *receiver = lookupTD(sendRequest->tid);

    // Check if the task exists
    if (receiver == nullptr || receiver->taskState == Constants::ZOMBIE) {
        return -1; // Task id did not correspond to a living task
    }

    activeTask->kSendRequest.receiverTD = receiver;
    activeTask->kSendRequest.sendRequest = sendRequest;

    if (receiver->taskState == Constants::RECEIVE_BLOCKED) {
        // Receiver is already ready for message to be copied
        *(receiver->kReceiveRequest.tid) = activeTask->tid;
        if (receiver->kReceiveRequest.msglen <= sendRequest->msglen) {
            memcpy(receiver->kReceiveRequest.msg, sendRequest->msg, receiver->kReceiveRequest.msglen);
            receiver->returnValue = receiver->kReceiveRequest.msglen;
        } else {
            memcpy(receiver->kReceiveRequest.msg, sendRequest->msg, sendRequest->msglen);
            receiver->returnValue = sendRequest->msglen;
        }

        // Transition the receiver to be ready and puts them on the ready queue
        receiver->taskState = Constants::READY;
        ready_queue.push(receiver, receiver->priority);
        // Transition the sender to be reply blocked
        replyQueue.push(&activeTask->kSendRequest);
        activeTask->taskState = Constants::REPLY_BLOCKED;

        return -3; // the return value will be set by handleReply
    } else {
        // Place sender on the receiver's receive queue
        receiver->receiveQueue.push(&activeTask->kSendRequest);

        // Transition the sender to be send blocked
        activeTask->taskState = Constants::SEND_BLOCKED;

        return -4; // In this case the return value will be set by handleReply
    }
}

int Kernel::handleReceive(int *tid, char *msg, int msglen) {
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
    for (unsigned int i = 0; i < replyQueue.size(); ++i) {
        kSendRequest = replyQueue.pop();
        if (kSendRequest->senderTD->tid == tid) {
            // if reply is truncated return -1 but copy what you can
            if (rplen > kSendRequest->sendRequest->rplen) {
                memcpy(kSendRequest->sendRequest->reply, reply, kSendRequest->sendRequest->rplen);
                kSendRequest->senderTD->returnValue = kSendRequest->sendRequest->rplen;
                kSendRequest->senderTD->taskState = Constants::READY;
                ready_queue.push(kSendRequest->senderTD, kSendRequest->senderTD->priority);

                return -1; // reply was truncated
            } else {
                memcpy(kSendRequest->sendRequest->reply, reply, rplen);
                kSendRequest->senderTD->returnValue = rplen; // Reply length was shorter so save that information
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

int Kernel::handleAwaitEvent(int eventId) {
    switch (eventId) {
        case Constants::TIMER_1_INTERRUPT:
            activeTask->taskState = Constants::TIMER_1_BLOCKED;
            break;

        case Constants::TIMER_2_INTERRUPT:
            activeTask->taskState = Constants::TIMER_2_BLOCKED;
            break;

        case Constants::TIMER_3_INTERRUPT:
            activeTask->taskState = Constants::TIMER_3_BLOCKED;
            break;

        case Constants::UART1RX_IRQ:
            activeTask->taskState = Constants::UART1RX_BLOCKED;
            break;

        case Constants::UART1TX_IRQ:
            activeTask->taskState = Constants::UART1TX_BLOCKED;
            break;

        case Constants::UART2RX_IRQ:
            activeTask->taskState = Constants::UART2RX_BLOCKED;
            break;

        case Constants::UART2TX_IRQ:
            activeTask->taskState = Constants::UART2TX_BLOCKED;
            break;

        default:
            bwprintf(COM2, "Kernel Handler - Unknown Await Event encountered: %d\n\r", eventId);
            return -1; // Invalid event should be returned from AwaitEvent
        }
    return 1;
}


// void Kernel::handleInterrupt(int data, DataStructures::RingBuffer<TaskDescriptor, Constants::NUM_TASKS> &blockedQueue) {
void Kernel::handleInterrupt(DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> &blockedQueue) {
    while(!blockedQueue.empty()) {
        TaskDescriptor* task = blockedQueue.pop();
        task->taskState = Constants::READY;
        // task->returnValue = data;
        ready_queue.push(task, task->priority);
    }
}

void Kernel::handleShutdown() {
    activeTask->taskState = Constants::STATE::ZOMBIE;
    while (!ready_queue.empty()) {
        TaskDescriptor* taskDescriptor = ready_queue.pop();
        taskDescriptor->taskState = Constants::STATE::ZOMBIE;
        exit_queue.push(taskDescriptor);
    }
}

int Kernel::handleHalt() {
    unsigned volatile int startIdleTaskTimeStamp = *(int *)(TIMER2_BASE + VAL_OFFSET);
    int haltAddress = 0x80930008;

    volatile int haltValue = * (int*)haltAddress;

    unsigned volatile int stopIdleTaskTimeStamp = *(int *)(TIMER2_BASE + VAL_OFFSET);
    return startIdleTaskTimeStamp - stopIdleTaskTimeStamp;
}

TaskDescriptor* Kernel::lookupTD(int tid) {
    if (tid < 0 || tid >= Constants::NUM_TASKS || tasks[tid].taskState == Constants::ZOMBIE) {
        return nullptr;
    }

    return &tasks[tid];
}
