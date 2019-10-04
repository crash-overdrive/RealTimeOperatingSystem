#include "Util.hpp"
#include "io/bwio.h"
#include "io/ts7200.h"
#include "user/syscall/UserSyscall.hpp"
#include "user/client/TimingTasks.hpp"
#include "kern/BootLoader.hpp"
#include "kern/Kernel.hpp"
#include "kern/Message.hpp"

extern "C" int* kernelExit(int stackPointer);

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
    asm volatile("ldr r12, =context_switch_enter");
    asm volatile("ldr r3, =0x28");
    asm volatile("str r12, [r3]");

    // Create the system's first task
    handleCreate(0, bootLoader);
}

void Kernel::schedule() {
    // TODO: what happens when ready_queue is empty?
    activeTask = ready_queue.pop();
}

int* Kernel::activate() {
    activeTask->sp[2] = activeTask->returnValue;

    int* stackPointer = kernelExit((int) activeTask->sp);
    activeTask->sp = stackPointer;
    return stackPointer;

}

void Kernel::handle(int* stackPointer)  {
    // Set the state of the activeTask to be READY
    // If it needs to be changed then the appropriate handler will do it
    activeTask->taskState = Constants::READY;

    int request = *(int*)(stackPointer[0] - 4) & 0xffffff;
    // bwprintf(COM2, "Got SWI: %d\n\r", request);
    
    void* arg1 = (void*)stackPointer[2];
    void* arg2 = (void*)stackPointer[3];
    void* arg3 = (void*)stackPointer[4];
    void* arg4 = (void*)stackPointer[5];

    switch(request) {
        case 2:
            activeTask->returnValue = handleCreate((int)arg1, (void (*)())arg2);
            break;

        case 3:
            activeTask->returnValue = handleMyTid();
            break;

        case 4:
            activeTask->returnValue = handleMyParentTid();
            break;

        case 5:
            break;

        case 6:
            handleExit();
            break;

        case 7:
            // bwprintf(COM2, "Active Task Tid before handleSend: %d\n\r", activeTask->tid);
            activeTask->returnValue = handleSend((SendRequest *) arg1);
            // bwprintf(COM2, "Active Task Tid after handleSend: %d\n\r", activeTask->tid);
            // bwprintf(COM2, "Came out from handlesend\n\r");
            break;

        case 8:
            activeTask->returnValue = handleReceive((int *) arg1, (int *) arg2, (int) arg3);
            // bwprintf(COM2, "Came out from handleReceive\n\r");
            break;

        case 9:
            activeTask->returnValue = handleReply((int) arg1, (const char *)arg2, (int) arg3);
            // bwprintf(COM2, "Came out from handleReply\n\r");
            break;

        default:
            bwprintf(COM2, "Invalid SWI: %d\n\r", request);
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


void Kernel::run() {
    int* stackPointer;
    initialize();
    FOREVER {
        schedule();
        if (activeTask == nullptr) { bwprintf(COM2, "No active tasks scheduled!"); break; }
        stackPointer = activate();
        handle(stackPointer);
    }
}
