#include <cstring>
#include "kern/Kernel.hpp"

int Kernel::handleCreate(int priority, void (*function)()) {
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
    newTD->stack[32766] = 0; // r4
    newTD->stack[32765] = 0; // r5
    newTD->stack[32764] = 0; // r6
    newTD->stack[32763] = 0; // r7
    newTD->stack[32762] = 0; // r8
    newTD->stack[32761] = 0; // r9
    newTD->stack[32760] = 0; // r10
    newTD->stack[32759] = 0; // r11
    newTD->stack[32758] = 0; // TODO: fix lr's values after we wrap function in another exit function

    newTD->sp = &(newTD->stack[32758]);

    ready_queue.push(newTD, newTD->priority);


    return availableTid;
}

int Kernel::handleMyTid() {
    return activeTask->tid;
}

int Kernel::handleMyParentTid() {
   return activeTask->parentTid;
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

TaskDescriptor* Kernel::lookupTD(int tid) {
    for (int i = 0; i < Constants::NUM_TASKS; ++i) {
        if (tasks[i].tid == tid) {
            if (tasks[i].taskState == Constants::ZOMBIE) { return nullptr; }
            return &tasks[i];
        }
    }
    return nullptr;
}