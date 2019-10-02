#include "Util.hpp"
#include "io/bwio.h"
#include "io/ts7200.h"
#include "user/syscall/UserSyscall.hpp"
#include "user/client/TimingTasks.hpp"
#include "kern/BootLoader.hpp"
#include "kern/Kernel.hpp"
#include "kern/Message.hpp"

volatile int reg0;
volatile int reg1;
volatile int reg2;
volatile int reg3;
volatile int reg4;
volatile int reg5;
volatile int reg6;
volatile int reg7;

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
    handleCreate(0, bootLoader);
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
            kernelRequestResponse = handleCreate((int)arg1, (void (*)())arg2);
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


void Kernel::run() {
    initialize();
    FOREVER {
        schedule();
        if (activeTask == nullptr) { bwprintf(COM2, "No active tasks scheduled!"); break; }
        request = activate();
        handle(request);
    }
}
