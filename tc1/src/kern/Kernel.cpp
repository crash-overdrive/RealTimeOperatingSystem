#include "Util.hpp"
#include "io/bwio.hpp"
#include "io/ep9302.h"
#include "io/ts7200.h"
#include "kern/BootLoader.hpp"
#include "kern/BootLoaderTest.hpp"
#include "kern/Kernel.hpp"
#include "kern/Message.hpp"
#include "user/syscall/UserSyscall.hpp"

extern "C" int* kernelExit(int stackPointer);

Kernel::Kernel(): uart1(UART1_BASE), uart2(UART2_BASE) {}

void Kernel::drawGUI() {
    bwprintf(COM2, Constants::VT100::MOVE_CURSOR_TO_HOME);
    bwprintf(COM2, Constants::VT100::CLEAR_SCREEN);
    bwprintf(COM2, "╔══════════════╦═════════════╦═══════════════════════════════════════════════════════════════╗\r\n"
                   "║ %sTime%s MM:SS.S ║ %sIdle%s PP.PP%% ║                            🚂 S-OS                            ║\r\n"
                   "╠══════════════╩═════════════╩═══════════════════════════════════════════════════════════════╣\r\n"
                   "║ %sSwitches%s                                                                                   ║\r\n"
                   "║   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18 153 154 155 156   ║\r\n"
                   "║                                                                                            ║\r\n"
                   "╠════════════════════════════════════════════════════════════════════════════════════════════╣\r\n"
                   "║ %sSensors%s                                                                                    ║\r\n"
                   "║   [                                         ]                                              ║\r\n"
                   "╠════════════════════════════════════════════════════════════════════════════════════════════╣\r\n"
                   "║ %sSensor Predictions%s                                                                         ║\r\n"
                   "║                                                                                            ║\r\n"
                   "║       N Sensor  Est Time  Est Dist  P Sensor  Real Time Real Dist                          ║\r\n"
                   "║   %sT1%s                                                                                       ║\r\n"
                   "║   %sT24%s                                                                                      ║\r\n"
                   "║   %sT58%s                                                                                      ║\r\n"
                   "║   %sT74%s                                                                                      ║\r\n"
                   "║   %sT78%s                                                                                      ║\r\n"
                   "║                                                                                            ║\r\n"
                   "╠════════════════════════════════════════════════════════════════════════════════════════════║\r\n"
                   "║ >                                                                                          ║\r\n"
                   "║                                                                                            ║\r\n"
                   "╠════════════════════════════════════════════════════════════════════════════════════════════╣\r\n"
                   "║ %sMessage Log%s                                                                                ║\r\n"
                   "║                                                                                            ║\r\n"
                   "║                                                                                            ║\r\n"
                   "║                                                                                            ║\r\n"
                   "║                                                                                            ║\r\n"
                   "╚════════════════════════════════════════════════════════════════════════════════════════════╝\r\n",
                   Constants::VT100::SET_UNDERSCORE, Constants::VT100::UNSET_UNDERSCORE,
                   Constants::VT100::SET_UNDERSCORE, Constants::VT100::UNSET_UNDERSCORE,
                   Constants::VT100::SET_UNDERSCORE, Constants::VT100::UNSET_UNDERSCORE,
                   Constants::VT100::SET_UNDERSCORE, Constants::VT100::UNSET_UNDERSCORE,
                   Constants::VT100::SET_UNDERSCORE, Constants::VT100::UNSET_UNDERSCORE,
                   "\033[32m", "\033[39m",
                   "\033[35m", "\033[39m",
                   "\033[33m", "\033[39m",
                   "\033[31m", "\033[39m",
                   "\033[34m", "\033[39m",
                   Constants::VT100::SET_UNDERSCORE, Constants::VT100::UNSET_UNDERSCORE);
    // These are for checking print positions, leave here for regression testing
    // bwprintf(COM2, Constants::VT100::MOVE_CURSOR_POS_TO_TIME);
    // bwprintf(COM2, "99:88.7");
    // bwprintf(COM2, Constants::VT100::MOVE_CURSOR_POS_TO_IDLE);
    // bwprintf(COM2, "98.21");
    // bwprintf(COM2, Constants::VT100::MOVE_CURSOR_POS_TO_SENSOR);
    // bwprintf(COM2, "[ C13 B14 A8 ]");
    // bwprintf(COM2, Constants::VT100::MOVE_CURSOR_POS_TO_MSGLOG);
    // bwprintf(COM2, "Everything is fine");
    bwprintf(COM2, Constants::VT100::MOVE_CURSOR_POS_TO_CONSOLE);
}

void Kernel::initialize() {
    // Setup comm
    uart1.setConfig(BPF8, OFF, ON, OFF);
	uart2.setConfig(BPF8, OFF, OFF, OFF);
    while(!uart1.isRXEmpty()) {
        uart1.getc();
    }
    while(!uart2.isRXEmpty()) {
        uart2.getc();
    }
    uart1.enableRXInterrupt();
    uart2.enableRXInterrupt();
    uart1.enableMISInterrupt();
    // uart2.enableTXInterrupt();

    // Draw GUI
    drawGUI();

    // Setup Switches
    // for (int i = 1; i <= 18; ++i) {
    //     uart1.putc(34);
    //     uart1.putc(i);
    // }

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
    // *(volatile int*)0x18 = 0xe59ff018; // e59ff018 = ldr pc, [pc, #24] TODO: Why doesn't this work???
    asm volatile("ldr r12, =handle_interrupt");
    asm volatile("ldr r3, =0x38");
    asm volatile("str r12, [r3]");

    // Create the system's first task
    handleCreate(0, bootLoader);
}

void Kernel::schedule() {
    activeTask = ready_queue.pop();
    // bwprintf(COM2, "Schedulding tid: %d to run with priority %d\n\r", activeTask->tid, activeTask->priority);
}

int* Kernel::activate() {
    if (activeTask->sp[0] == 0) {
        activeTask->sp[3] = activeTask->returnValue;
    } else if (activeTask->sp[0] != 1) {
        bwprintf(COM2, "Kernel - Activate - ActiveTask sp[0] error: %d\n\r", activeTask->sp[0]);
    }

    // Exit Kernel
    int* stackPointer = kernelExit((int) activeTask->sp);
    activeTask->sp = stackPointer;

    return stackPointer;
}

void Kernel::handle(int* stackPointer)  {
    // Set the state of the activeTask to be READY
    // If it needs to be changed then the appropriate handler will do it
    activeTask->taskState = Constants::STATE::READY;

    // Handle Hardware Interrupts
    if (stackPointer[0]) {
        int vic1Status = *(int *)(VIC1_IRQ_BASE + IRQ_STATUS_OFFSET);
        int vic2Status = *(int *)(VIC2_IRQ_BASE + IRQ_STATUS_OFFSET);
        // bwprintf(COM2, "Kernel - Hardware interrupt vic1Status %d vic2Status %d \n\r", vic1Status, vic2Status);

        if (vic1Status & TC1UI_MASK) {
            *(int *)(TIMER1_BASE + CLR_OFFSET) = 1; // Clear the interrupt
            handleInterrupt(timer1BlockedQueue);
            timeSpentInIdleInLastTick = timeSpentInIdleInThisTick;
            timeSpentInIdleInThisTick = 0;
        } else if (vic1Status & TC2UI_MASK) {
            *(int *)(TIMER2_BASE + CLR_OFFSET) = 1;
            handleInterrupt(timer2BlockedQueue);
        } else if (vic2Status & TC3UI_MASK) {
            *(int *)(TIMER3_BASE + CLR_OFFSET) = 1;
            handleInterrupt(timer3BlockedQueue);
        // } else if (vic1Status & UART1_RX_INTR1_MASK) {
        //     uart1.disableRXInterrupt();
        //     handleInterrupt(uart1RXBlockedQueue);
        // } else if (vic1Status & UART1_TX_INTR1_MASK) {
        //     uart1.disableTXInterrupt();
        //     handleInterrupt(uart1TXBlockedQueue);
        } else if (vic1Status & UART2_RX_INTR2_MASK) {
            uart2.disableRXInterrupt();
            handleInterrupt(uart2RXBlockedQueue);
        } else if (vic1Status & UART2_TX_INTR2_MASK) {
            uart2.disableTXInterrupt();
            handleInterrupt(uart2TXBlockedQueue);
        // This is here as an artefact of exploration
        } else if (vic2Status & INT_UART1_MASK) {
            if (uart1.isRXInterrupt()) {
                uart1.disableRXInterrupt();
                uart1.clearRXInterrupt();
                handleInterrupt(uart1RXBlockedQueue);
            } else if (uart1.isTXInterrupt()) {
                uart1.disableTXInterrupt();
                uart1.clearTXInterrupt();
                if (uart1.isMISInterrupt()) {
                    if (uart1.isClearToSend()) {
                        bwprintf(COM2, "Kernel - CTS quirk observed!\r\n");
                        handleInterrupt(uart1TXBlockedQueue);
                    } else {
                        bwprintf(COM2, "Kernel - Does this ever occur?\r\n");
                        // Transition to waiting for CTS_UP
                    }
                    uart1.clearMISInterrupt();
                } else {
                    // Transition to waiting for MIS
                }
            } else if (uart1.isMISInterrupt()) {
                // Transition to waiting for CTS_DOWN
                if (uart1.isClearToSend()) {
                    handleInterrupt(uart1TXBlockedQueue);
                } else {
                    // Transition to waiting for CTS_UP
                }
                uart1.clearMISInterrupt();
            }
        } else {
            bwprintf(COM2, "Kernel - ERROR: Kernel interrupted with unknown interrupt\n\r");
            TRAP
        }

    } else { // Handle Software Interrupts

        int request = *(int*)(stackPointer[1] - 4) & 0xffffff;

        void* arg1 = (void*)stackPointer[3];
        void* arg2 = (void*)stackPointer[4];
        void* arg3 = (void*)stackPointer[5];

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
                activeTask->returnValue = handleSend((SendRequest *) arg1);
                break;

            case Constants::SWI::RECIEVE:
                activeTask->returnValue = handleReceive((int *) arg1, (char *) arg2, (int) arg3);
                break;

            case Constants::SWI::REPLY:
                activeTask->returnValue = handleReply((int) arg1, (const char *)arg2, (int) arg3);
                break;

            case Constants::SWI::AWAIT_EVENT:
                activeTask->returnValue = handleAwaitEvent((int)arg1);
                break;

            case Constants::SWI::SWITCH_OFF:
                handleSwitchOff();
                break;

            case Constants::SWI::HALT:
                activeTask->returnValue = timeSpentInIdleInLastTick;
                timeSpentInIdleInThisTick = timeSpentInIdleInThisTick + handleHalt();
                break;

            default:
                bwprintf(COM2, "Kernel - Invalid SWI: %d\n\r", request);
                break;
        }
    }

    // bwprintf(COM2, "Task state is: %d %d\n\r", activeTask->tid, activeTask->taskState);
    switch (activeTask->taskState) {
        case Constants::STATE::READY:
            // bwprintf(COM2, "Pushing %d to ready queue\n\r", activeTask->tid);
            ready_queue.push(activeTask, activeTask->priority);
            break;

        case Constants::STATE::ZOMBIE:
            exit_queue.push(activeTask);
            break;

        case Constants::STATE::ACTIVE:
            bwprintf(COM2, "Kernel - Task shouldn't have ACTIVE state\n\r");
            break;

        case Constants::STATE::SEND_BLOCKED:
            // bwprintf(COM2, "TID: %d on SEND_BLOCKED\n\r", activeTask->tid);
            break;
        case Constants::STATE::RECEIVE_BLOCKED:
            // bwprintf(COM2, "TID: %d on RECEIVE_BLOCKED\n\r", activeTask->tid);
            break;
        case Constants::STATE::REPLY_BLOCKED:
            // bwprintf(COM2, "TID: %d on REPLY_BLOCKED\n\r", activeTask->tid);
            break;

        case Constants::STATE::TIMER_1_BLOCKED:
            // bwprintf(COM2, "Kernel - Putting %d on timerBlockedQueue\n\r", activeTask->tid);
            timer1BlockedQueue.push(activeTask);
            break;

        case Constants::STATE::TIMER_2_BLOCKED:
            // bwprintf(COM2, "Kernel - Putting %d on timerBlockedQueue\n\r", activeTask->tid);
            timer2BlockedQueue.push(activeTask);
            break;

        case Constants::STATE::TIMER_3_BLOCKED:
            // bwprintf(COM2, "Kernel - Putting %d on timerBlockedQueue\n\r", activeTask->tid);
            timer3BlockedQueue.push(activeTask);
            break;

        case Constants::STATE::UART1RX_BLOCKED:
            uart1RXBlockedQueue.push(activeTask);
            break;

        case Constants::STATE::UART1TX_BLOCKED:
            uart1TXBlockedQueue.push(activeTask);
            break;

        case Constants::STATE::UART2RX_BLOCKED:
            // bwprintf(COM2, "Kernel - Putting %d on UART2RXBlockedQueue\n\r", activeTask->tid);
            uart2RXBlockedQueue.push(activeTask);
            break;

        case Constants::STATE::UART2TX_BLOCKED:
            // bwprintf(COM2, "Kernel - Putting %d on UART2RXBlockedQueue\n\r", activeTask->tid);
            uart2TXBlockedQueue.push(activeTask);
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
        // bwprintf(COM2, "Kernel - %d %d\n\r", uart1.getInterruptStatus(), uart2.getInterruptStatus());
        schedule();
        if (activeTask == nullptr) {
            bwprintf(COM2, "Kernel - No active tasks scheduled, Shutting down!\n\r");
            // asm volatile("msr cpsr_c, #0b11010011");
            break;
        }
        stackPointer = activate();
        handle(stackPointer);
    }
}
