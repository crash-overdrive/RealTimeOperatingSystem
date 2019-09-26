#include "../../include/Kernel.hpp"

void Kernel::initialize() {
    // Setup comm
    uart.setConfig(COM1, BPF8, OFF, ON, OFF);
	uart.setConfig(COM2, BPF8, OFF, OFF, OFF);

    // Setup 0x8, 0x28
    *(int*)0x8 = 0xe59ff018; // e59ff018 = ldr pc, [pc, #24]
    // asm volatile("mov r12, #0xe59ff018"); // e59ff018 = ldr pc, [pc, #24]
    // asm volatile("str r12, #0x8");
    asm volatile("ldr r12, =kernel_entry");
    asm volatile("ldr r3, =0x28");
    asm volatile("str r12, [r3]");

    // TODO: Setup first task
    
    handleCreate(1, firstTask);
}

void Kernel::schedule() {
    // TODO: what happens when ready_queue is empty?
    activeTask = ready_queue.pop();
}

int Kernel::activate() {

    // Kernel exits from here!!
    kernel_exit:

    // SUPERVISOR MODE
    // Move cpsr of active task into spsr_svc
    asm volatile("msr spsr, %r0" :: "r"(activeTask->cpsr));

    // SUPERVISOR MODE
    // store kernel state on kernel stack
    asm volatile("stmfd sp!, {r0-r12, lr}");
    asm volatile("mrs r12, cpsr");
    asm volatile("stmfd sp!, {r12}");

    // SUPERVISOR MODE
    // Move the next pc into lr_svc
    asm volatile("mov lr, %r0" :: "r"(activeTask->pc));

    // SUPERVISOR MODE
    // Change to system mode
    asm volatile("mov r12, #0b11111");
    asm volatile("msr cpsr, r12");

    // SYSTEM MODE
    // TODO:implement this
    // Load registers from user stack
    asm volatile("ldmfd sp!, {r4-r11, lr}");

    // SYSTEM MODE
    // Return to supervisor mode
    asm volatile("mov r12, #0b10011");
    asm volatile("msr cpsr, r12");

    // SUPERVISOR MODE
    // Set return value by overwriting r0
    asm volatile("mov r0, %r0" :: "r"(activeTask->r0));

    // SUPERVISOR MODE
    // Go back to user mode
    asm volatile("movs pc, lr");
    // !!!!!!!!!!!!KERNEL EXITED!!!!!!!!!!!!




    // Should never reach here!! some bug if it reached here!
    Util::assert(false);




    // !!!!!!!!!!!!KERNEL ENTERS!!!!!!!!!!!!
    asm volatile("kernel_entry: ");
    
    // SUPERVISOR MODE
    // Change to system mode
    asm volatile("mov r12, #0b11111");
    asm volatile("msr cpsr, r12");
    
    // SYSTEM MODE
    // Save user state (r4-r11, lr) on user stack  
    // r12 saved by caller
    // sp, pc should not be saved on the stack
    // sp is decremented appropriately and saved into the task descriptor
    // cpsr for user mode is saved in spsr_svc, need to save that into task descriptor as well!!
    asm volatile("stmfd sp!, {r4-r11, lr}");
    asm volatile("mov %r0, sp" : "=r"(activeTask->sp));

    // SYSTEM MODE
    // Return to supervisor mode
    asm volatile("mov r12, #0b10011");
    asm volatile("msr cpsr, r12");

    // SUPERVISOR MODEvoid;
    // Saving cspr of user mode into task descriptor
    asm volatile("mrs %r0, spsr" : "=r"(activeTask->cpsr));

    // SUPERVISOR MODE
    // Saving pc of user mode into task descriptor
    asm volatile("mov %r0, lr" : "=r"(activeTask->pc));

    // SUPERVISOR MODE
    // Get Request typestmfd
    int requestCode;
    asm volatile("ldr r4, [lr, #-4]");
    asm volatile("mov %r0, r4" : "=r"(requestCode));

    // SUPERVISOR MODE
    // Get all arguments
    asm volatile("mov %r0, r0" : "=r"(arg1));
    asm volatile("mov %r0, r1" : "=r"(arg2));
    asm volatile("mov %r0, r2" : "=r"(arg3));
    asm volatile("mov %r0, r3" : "=r"(arg4));

    // SUPERVISOR MODE
    // Retrieve Kernal State from Kernal Stack
    asm volatile("ldmfd sp!, {r12}");
    asm volatile("msr cpsr, r12");
    asm volatile("ldmfd sp!, {r0-r12, lr}");
    
    // SUPERVISOR MODE
    // return requestCode - End of function
    return requestCode;
    
}

void Kernel::handle(int request)  {
    // Set the state of the activeTask to be READY
    // If it needs to be changed then the appropriate handler will do it
    activeTask->taskState = Constants::READY;

    switch(request) {
        int kernelRequestResponse;
        case 2:
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

        default:
            bwprintf(COM2, "Invalid argument to SWI passed: %d \n", request);
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
            bwprintf(COM2, "Task shouldnt have ACTIVE state\n");
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
    newTD->parentTid = activeTask->tid;
    newTD->priority = priority;
    newTD->taskState = Constants::READY;
    newTD->r0 = 0;

    // TODO: check validity of cpsr and pc
    // TODO: wrap function in another function with exit()
    newTD->pc = (int)function;
    

    // setting the stack [r4-r11, lr]
    
    newTD->stack[0] = 0xdeadbeef; // for debugging purposes
    newTD->stack[1] = 0; // r4
    newTD->stack[2] = 0; // r5
    newTD->stack[3] = 0; // r6
    newTD->stack[4] = 0; // r7
    newTD->stack[5] = 0; // r8
    newTD->stack[6] = 0; // r9
    newTD->stack[7] = 0; // r10
    newTD->stack[8] = 0; // r11
    newTD->stack[9] = 0; // TODO: fix lr

    newTD->sp = &(newTD->stack[9]);

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
    int a = 1;
    int b = 2;
    int c = a + b;
    bwprintf(COM2, "Value of c: %d\n", c);
}

void Kernel::run() {
    initialize();
    FOREVER {
        schedule();
        if (activeTask == nullptr) { continue; }
        request = activate();
        handle(request);
    }
    bwprintf(COM2, "It doesn't work!");
}
