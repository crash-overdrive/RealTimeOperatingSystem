#include "Util.hpp"
#include "io/bwio.hpp"
#include "io/ep9302.h"
#include "io/ts7200.h"
#include "kern/BootLoader.hpp"
#include "kern/Halt.hpp"
#include "kern/Kernel.hpp"
#include "kern/Message.hpp"
#include "user/client/TimingTasks.hpp"
#include "user/syscall/UserSyscall.hpp"

extern "C" int* kernelExit(int stackPointer);

void Kernel::initialize() {
    // Setup comm
    uart.setConfig(COM1, BPF8, OFF, ON, OFF);
	uart.setConfig(COM2, BPF8, OFF, OFF, OFF);

    // Start kernel clock (Timer3 is reserved by the kernel)
    *(int *)(TIMER3_BASE + LDR_OFFSET) = 0xFFFFFFFF;
    *(int *)(TIMER3_BASE + CRTL_OFFSET) |= ENABLE_MASK | CLKSEL_MASK;
    *(int *)(TIMER3_BASE + CRTL_OFFSET) &= ~MODE_MASK;

    // Draw GUI
    drawGUI();

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

    // Create the system's idle task and intialize timing
    haltTD = lookupTD(handleCreate(Constants::NUM_PRIORITIES-1, halt));
    haltActivate = haltReturn = lastHaltActivate = *(int *)(TIMER3_BASE + VAL_OFFSET);
    updateTick = 0;
}

void Kernel::drawGUI() {
    // TODO: refactor this into a graphics context maybe?
    // bwprintf(COM2, "\0337\033[3;40f╔═════════════════════╗");
    // bwprintf(COM2, "\033[4;40f║ \033[4mIdle Time:\033[24m   \033[41m0.000%\033[47m ║");
    // bwprintf(COM2, "\033[5;40f╚═════════════════════╝\0338");

    bwprintf(COM2, "\0337\033[2J\033[3;47f\033[37m╔═════════════════════╗");
    bwprintf(COM2, "\033[4;47f║ \033[4mIdle Time:\033[24m   \033[31m0.000%%\033[37m ║");
    bwprintf(COM2, "\033[5;47f╚═════════════════════╝\0338");
}

void Kernel::displayIdle(unsigned int idlePercent) {
    // -> dddddd
    // -> ddd.ddd
    if (idlePercent < 50000) {
        // The column is +14 from the location we use in the gui
        bwprintf(COM2, "\0337\033[1;47f\033[K");
        bwprintf(COM2, "\033[2;47f\033[K");
        bwprintf(COM2, "\033[3;47f\033[37m╔═════════════════════╗");
        bwprintf(COM2, "\033[4;47f║ \033[4mIdle Time:\033[24m  \033[31m%d.%d%%\033[37m ║\033[K", idlePercent/1000, idlePercent%1000);
        bwprintf(COM2, "\033[5;47f╚═════════════════════╝\0338");
        // bwprintf(COM2, "\0337\033[4;17f\033[31m%d.%d%%\0338", idlePercent/1000, idlePercent%1000);
        // bwprintf(COM2, "   RED: Idle percent %d\n\r", idlePercent);
    } else if (idlePercent < 80000) {
        bwprintf(COM2, "\0337\033[1;47f\033[K");
        bwprintf(COM2, "\033[2;47f\033[K");
        bwprintf(COM2, "\033[3;47f\033[37m╔═════════════════════╗");
        bwprintf(COM2, "\033[4;47f║ \033[4mIdle Time:\033[24m  \033[33m%d.%d%%\033[37m ║\033[K", idlePercent/1000, idlePercent%1000);
        bwprintf(COM2, "\033[5;47f╚═════════════════════╝\0338");
        // bwprintf(COM2, "\0337\033[4;17f\033[33m%d.%d%%\0338", idlePercent/1000, idlePercent%1000);
        // bwprintf(COM2, "YELLOW: Idle percent %d\n\r", idlePercent);
    } else {
        bwprintf(COM2, "\0337\033[1;47f\033[K");
        bwprintf(COM2, "\033[2;47f\033[K");
        bwprintf(COM2, "\033[3;47f\033[37m╔═════════════════════╗");
        bwprintf(COM2, "\033[4;47f║ \033[4mIdle Time:\033[24m  \033[32m%d.%d%%\033[37m ║\033[K", idlePercent/1000, idlePercent%1000);
        bwprintf(COM2, "\033[5;47f╚═════════════════════╝\0338");
        // bwprintf(COM2, "\0337\033[4;17f\033[32m%d.%d%%\0338", idlePercent/1000, idlePercent%1000);
        // bwprintf(COM2, " GREEN: Idle percent %d\n\r", idlePercent);
    }
}

void Kernel::schedule() {
    // TODO: what happens when ready_queue is empty?
    activeTask = ready_queue.pop();
    // bwprintf(COM2, "Schedulding tid: %d to run with priority %d\n\r", activeTask->tid, activeTask->priority);
}


void printStack(int* stackPointer) {
    for (int i = 0; i < 45; ++i) {
        bwprintf(COM2, "%d - %d\n\r", i, stackPointer[i]);
    }
}

int* Kernel::activate() {
    activeTask->sp[3] = activeTask->returnValue;
    if (activeTask == haltTD) {
        lastHaltActivate = haltActivate;
        haltActivate = *(unsigned int *)(TIMER3_BASE + VAL_OFFSET);
        // Sadly, GCC clobbers the shit out of this update loop which was intended to make it so that we only re-render every 100ms, RIP F
        // updateTick += lastHaltActivate - haltActivate;
        // // bwprintf(COM2, "update tick? %d \r\n", updateTick);
        // if (updateTick > 50800) {
        //     displayIdle((volatile unsigned long long)(lastHaltActivate - haltReturn) * 100000 / (lastHaltActivate - haltActivate));
        //     updateTick -= 50800;
        // }
        // The cast here is to prevent overflow during the multiplacation. It might have been possible to avoid this, but we wanted 3 decimal precision.
        displayIdle((unsigned long long)(lastHaltActivate - haltReturn) * 100000 / (lastHaltActivate - haltActivate));
    }

    // Exit Kernel
    int* stackPointer = kernelExit((int) activeTask->sp);
    activeTask->sp = stackPointer;

    if (activeTask == haltTD) {
        haltReturn = *(int *)(TIMER3_BASE + VAL_OFFSET);
    }

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
            // bwprintf(COM2, "Kernel - The interrupt was a timer 1 underflow interrupt\n\r");
            *(int *)(TIMER1_BASE + CLR_OFFSET) = 1; // Clear the interrupt
            handleTimerInterrupt(1);
            // handleTimerUnderflow(1);
        } else if (vic1Status & TC2UI_MASK) {
            // bwprintf(COM2, "Kernel - The interrupt was a timer 2 underflow interrupt\n\r");
            *(int *)(TIMER2_BASE + CLR_OFFSET) = 1;
            // handleTimerUnderflow(2);
        } else if (vic2Status & TC3UI_MASK) {
            // bwprintf(COM2, "Kernel - The interrupt was a timer 3 overflow interrupt!\n\r");
            *(int *)(TIMER3_BASE + CLR_OFFSET) = 1;
            // handleTimerUnderflow(3);
        } else {
            bwprintf(COM2, "Kernel - ERROR: Kernel interrupted with unknown interrupt\n\r");
            TRAP
        }

    } else {

        int request = *(int*)(stackPointer[1] - 4) & 0xffffff;
        volatile int halt = 0;
        // bwprintf(COM2, "Got SWI: %d\n\r", request);
        // bwprintf(COM2, "Software Interrupt: %d\n\r", request);

        void* arg1 = (void*)stackPointer[3];
        void* arg2 = (void*)stackPointer[4];
        void* arg3 = (void*)stackPointer[5];
        void* arg4 = (void*)stackPointer[6];

        switch(request) {
            case Constants::SWI::CREATE:
                activeTask->returnValue = handleCreate((int)arg1, (void (*)())arg2);
                break;

            case Constants::SWI::MY_TID:
                activeTask->returnValue = handleMyTid();
                break;

            case Constants::SWI::MY_PARENT_TID:
                activeTask->returnValue = handleMyParentTid();
                break;

            case Constants::SWI::YIELD:
                break;

            case Constants::SWI::EXIT:
                handleExit();
                break;

            case Constants::SWI::SEND:
                // bwprintf(COM2, "Active Task Tid before handleSend: %d\n\r", activeTask->tid);
                activeTask->returnValue = handleSend((SendRequest *) arg1);
                // bwprintf(COM2, "Active Task Tid after handleSend: %d\n\r", activeTask->tid);
                // bwprintf(COM2, "Came out from handlesend\n\r");
                break;

            case Constants::SWI::RECIEVE:
                activeTask->returnValue = handleReceive((int *) arg1, (int *) arg2, (int) arg3);
                // bwprintf(COM2, "Came out from handleReceive\n\r");
                break;

            case Constants::SWI::REPLY:
                activeTask->returnValue = handleReply((int) arg1, (const char *)arg2, (int) arg3);
                // bwprintf(COM2, "Came out from handleReply\n\r");
                break;

            case Constants::SWI::AWAIT_EVENT:
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
            // bwprintf(COM2, "Kernel - Putting %d on timerBlockedQueue\n\r", activeTask->tid);
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
    FOREVER {
        schedule();
        if (activeTask == nullptr) { 
            bwprintf(COM2, "Kernel - No active tasks scheduled!");
            // asm volatile("msr cpsr_c, #0b11010011");
            break; 
        }
        stackPointer = activate();
        handle(stackPointer);
    }
}
