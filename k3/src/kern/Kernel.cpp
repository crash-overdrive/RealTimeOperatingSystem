#include "Util.hpp"
#include "io/bwio.hpp"
#include "io/ep9302.h"
#include "io/ts7200.h"
#include "user/client/TimingTasks.hpp"
#include "user/syscall/UserSyscall.hpp"
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

    // TODO: Refactor interrupt setup to be in Config.s
    
    // Setup 0x8, 0x28
    // asm volatile("mov r12, #0xe59ff018"); // e59ff018 = ldr pc, [pc, #24]
    // asm volatile("str r12, #0x8");
    *(volatile int*)0x8 = 0xe59ff018; // e59ff018 = ldr pc, [pc, #24]
    asm volatile("ldr r12, =context_switch_enter");
    asm volatile("ldr r3, =0x28");
    asm volatile("str r12, [r3]");

    // Setup 0x18, 0x38
    // asm volatile("TEST: ");
    // *(volatile int*)0x18 = 0xe59ff018; // e59ff018 = ldr pc, [pc, #24] TODO: Why doesn't this work???
    asm volatile("ldr r12, =handle_interrupt");
    asm volatile("ldr r3, =0x38");
    asm volatile("str r12, [r3]");

    // Create the system's first task
    handleCreate(0, bootLoader);
}

void Kernel::schedule() {
    // TODO: what happens when ready_queue is empty?
    activeTask = ready_queue.pop();
    // bwprintf(COM2, "Schedulding tid: %d to run\n\r", activeTask->tid);
}


void printStack(int* stackPointer) {
    for (int i = 0; i < 45; ++i) {
        bwprintf(COM2, "%d - %d\n\r", i, stackPointer[i]);
    }
}

int* Kernel::activate() {
    activeTask->sp[3] = activeTask->returnValue;
    // bwprintf(COM2, "Value of lr is: %d\n\r", activeTask->sp[16]);
    // printStack(activeTask->sp);
    int* stackPointer = kernelExit((int) activeTask->sp);
    // bwprintf(COM2, "Value of lr is: %d\n\r", stackPointer[16]);
    activeTask->sp = stackPointer;

    return stackPointer;
}

void Kernel::handle(int* stackPointer)  {
    // Set the state of the activeTask to be READY
    // If it needs to be changed then the appropriate handler will do it
    activeTask->taskState = Constants::READY;
    

    if (stackPointer[0]) {
        int vic1Status = *(int *)(VIC1_IRQ_BASE + IRQ_STATUS_OFFSET);
        int vic2Status = *(int *)(VIC1_IRQ_BASE + IRQ_STATUS_OFFSET);

        if (vic1Status & TC1UI_MASK) {
            bwprintf(COM2, "Kernel - The interrupt was a timer 1 underflow interrupt\n\r");
            *(int *)(TIMER1_BASE + CLR_OFFSET) = 1; // Clear the interrupt
            handleTimerInterrupt(1);
            // handleTimerUnderflow(1);
        } else if (vic1Status & TC2UI_MASK) {
            bwprintf(COM2, "Kernel - The interrupt was a timer 2 underflow interrupt\n\r");
            *(int *)(TIMER2_BASE + CLR_OFFSET) = 1;
            // handleTimerUnderflow(2);
        } else if (vic2Status & TC3UI_MASK) {
            bwprintf(COM2, "Kernel - The interrupt was a timer 3 underflow interrupt\n\r");
            *(int *)(TIMER3_BASE + CLR_OFFSET) = 1;
            // handleTimerUnderflow(3);
        } else {
            bwprintf(COM2, "Kernel - ERROR: Kernel interrupted with unknown interrupt\n\r");
            TRAP
        }

    } else {
        
        int request = *(int*)(stackPointer[1] - 4) & 0xffffff;
        // bwprintf(COM2, "Got SWI: %d\n\r", request);
        // bwprintf(COM2, "Software Interrupt: %d\n\r", request);

        void* arg1 = (void*)stackPointer[3];
        void* arg2 = (void*)stackPointer[4];
        void* arg3 = (void*)stackPointer[5];
        void* arg4 = (void*)stackPointer[6];

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

            case 10:
                activeTask->returnValue = handleAwaitEvent((int)arg1);
                break;
            default:
                bwprintf(COM2, "Kernel - Invalid SWI: %d\n\r", request);
                break;
        }
    }

    // bwprintf(COM2, "Task state is: %d %d\n\r", activeTask->tid, activeTask->taskState);
    switch (activeTask->taskState) {
        case Constants::READY:
            // bwprintf(COM2, "Pushing %d to ready queue\n\r", activeTask->tid);
            ready_queue.push(activeTask, activeTask->priority);
            break;

        case Constants::ZOMBIE:
            exit_queue.push(activeTask);
            break;

        case Constants::ACTIVE:
            bwprintf(COM2, "Kernel - Task shouldn't have ACTIVE state\n\r");
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

        case Constants::TIMER_BLOCKED:
            bwprintf(COM2, "Kernel - Putting %d on timerBlockedQueue\n\r", activeTask->tid);
            timerBlockedQueue.push(activeTask);
            break;

        default:
            bwprintf(COM2, "Kernel - Received invalid task State: %d \n\r", activeTask->taskState);
            break;
    }
    
}


void Kernel::run() {
    int* stackPointer;
    initialize();
    *(int *)(TIMER1_BASE + LDR_OFFSET) = 2000; // This is just for testing interrupts
    *(int *)(TIMER1_BASE + CRTL_OFFSET) = ENABLE_MASK | MODE_MASK; // | CLKSEL_MASK;
    FOREVER {
        schedule();
        // Testing interrupts
        // bwprintf(COM2, "THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG!THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG! THIS MESSAGE IS INTENTIONALLY LONG!\n\r");
        // bwprintf(COM2, "TIMER CTRL: %d\n\r", *(int *)(TIMER1_BASE + CRTL_OFFSET));
        // bwprintf(COM2, "TIMER VALUE: %d\n\r", *(int *)(TIMER1_BASE + VAL_OFFSET));
        // Testing interrupts
        if (activeTask == nullptr) { 
            bwprintf(COM2, "Kernel - No active tasks scheduled!"); 
            // asm volatile("msr cpsr_c, #0b11010011");
            break; 
        }
        stackPointer = activate();
        handle(stackPointer);
        // bwprintf(COM2, "Got here\n\r");
    }
}

