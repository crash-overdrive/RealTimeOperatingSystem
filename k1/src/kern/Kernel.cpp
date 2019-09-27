#include "../../include/Kernel.hpp"
#include "../../include/UserSyscall.hpp"

#define print(str) bwprintf(COM2, str)

int reg0;
int reg1;
int reg2;
int reg3;
int reg4;
int reg5;
int reg6;
int reg7;

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

void Kernel::initialize() {
    
    // Setup comm
    uart.setConfig(COM1, BPF8, OFF, ON, OFF);
	uart.setConfig(COM2, BPF8, OFF, OFF, OFF);

    // Setup 0x8, 0x28
    *(int*)0x8 = 0xe59ff018; // e59ff018 = ldr pc, [pc, #24]
    // asm volatile("mov r12, #0xe59ff018"); // e59ff018 = ldr pc, [pc, #24]
    // asm volatile("str r12, #0x8");
    asm volatile("ldr r12, =context_switch_entry");
    asm volatile("ldr r3, =0x28");
    asm volatile("str r12, [r3]");

    // TODO: Setup first task
    
    handleCreate(2, firstTask);
}

void Kernel::schedule() {
    // TODO: what happens when ready_queue is empty?
    activeTask = ready_queue.pop();
    // bwprintf(COM2, "Scheduled %d\n", activeTask->tid);
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
    activeTask->cpsr = reg6;
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
            bwprintf(COM2, "Called Create \n");
            kernelRequestResponse = handleCreate((int)arg1, (int (*)())arg2);
            activeTask->r0 = kernelRequestResponse;
            break;

        case 3:
            bwprintf(COM2, "Called MyTid \n");
            kernelRequestResponse = handleMyTid();
            activeTask->r0 = kernelRequestResponse;
            break;

        case 4:
            bwprintf(COM2, "Called MyParentTid \n");
            kernelRequestResponse = handleMyParentTid();
            activeTask->r0 = kernelRequestResponse;
            break;

        case 5:
            bwprintf(COM2, "Called Yield \n");
            break;

        case 6:
            bwprintf(COM2, "Called Exit \n");
            handleExit();
            break;

        default:
            bwprintf(COM2, "Invalid argument to SWI passed: %d \n", request);
            break;
    }

    switch (activeTask->taskState) {
        case Constants::READY:
            ready_queue.push(activeTask, activeTask->priority);
            break;

        case Constants::ZOMBIE:
            bwprintf(COM2, "Pushing to dead queue");
            exit_queue.push(activeTask);
            break;

        case Constants::ACTIVE:
            bwprintf(COM2, "Task shouldnt have ACTIVE state\n");
            break;

        default:
            bwprintf(COM2, "Got invalid task State: %d \n", activeTask->taskState);
            break;
    }
    
}

int Kernel::handleCreate(int priority, int (*function)()) {
    // bwprintf(COM2, "Enter handle create\n\r");
    taskNumber++;
    availableTid++;

    if (priority < 0 || priority >= Constants::NUM_PRIORITIES) {
        return -1;
    }

    if (taskNumber >= Constants::NUM_TASKS) {
        return -2;  
    }
    // bwprintf(COM2, "Clear check!\n\r");
    
    TaskDescriptor* newTD = &tasks[taskNumber];

    newTD->tid = availableTid;

    if (activeTask == nullptr) {
        newTD->parentTid = 0;
    } else {
        newTD->parentTid = activeTask->tid;
    }
    // bwprintf(COM2, "TID: %d\n\r", newTD->tid);
    
    newTD->priority = priority;
    newTD->taskState = Constants::READY;
    newTD->r0 = 0;
    newTD->cpsr = 0b10000;

    // TODO: check validity of pc
    // TODO: wrap function in another function with exit()
    newTD->pc = (int)function;

    // setting the stack [r4-r11, lr]
    
    newTD->stack[32767] = 0xdeadbeef; // for debugging purposes
    newTD->stack[32766] = 1; // r4
    newTD->stack[32765] = 2; // r5
    newTD->stack[32764] = 3; // r6
    newTD->stack[32763] = 4; // r7
    newTD->stack[32762] = 5; // r8
    newTD->stack[32761] = 6; // r9
    newTD->stack[32760] = 7; // r10
    newTD->stack[32759] = 8; // r11
    newTD->stack[32758] = 9; // TODO: fix lrs values

    // bwprintf(COM2, "SP UPDATE?\n\r");
    newTD->sp = &(newTD->stack[32758]);

    // bwprintf(COM2, "Value of SP for new user task is %x\n", newTD->sp);
    // bwprintf(COM2, "RQ PUSH?\n\r");
    ready_queue.push(newTD, newTD->priority);
    // bwprintf(COM2, "Exit handle create\n\r");

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

int Kernel::firstTask() {
    int tid;
    tid = Create(3, testTask);
    bwprintf(COM2, "FirstUserTask: Created: %d\n\r", tid);
    tid = Create(3, testTask);
    bwprintf(COM2, "FirstUserTask: Created: %d\n\r", tid);
    tid = Create(1, testTask);
    bwprintf(COM2, "FirstUserTask: Created: %d\n\r", tid);
    tid = Create(1, testTask);
    bwprintf(COM2, "FirstUserTask: Created: %d\n\r", tid);
    bwprintf(COM2, "FirstUserTask: exiting");
    Exit();
}

void Kernel::run() {
    int i = 0;
    initialize();
    FOREVER {
        if (i == 5) { break; }
        schedule();
        if (activeTask == nullptr) { bwprintf(COM2, "BREAK!"); break; }
        request = activate();
        handle(request);
        i++;
    }
}
