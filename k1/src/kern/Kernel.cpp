#include "../../include/Kernel.hpp"

void Kernel::initialize() {
    // TODO: implement me
    // setup comm
    // setup 0x08, 0x28
    // setup first task
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
    // Move the next pc into lr_svcstmfd
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
    asm volatile("mov r0, %r0" :: "r"(activeTask->returnValue));

    // SUPERVISOR MODE
    // Go back to user mode
    asm volatile("movs pc, lr");
    // !!!!!!!!!!!!KERNEL EXITED!!!!!!!!!!!!




    // Should never reach here!! some bug if it reached here!
    Util::assert(false);




    // !!!!!!!!!!!!KERNEL ENTERS!!!!!!!!!!!!
    kernel_entry:
    
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
    asm volatile("mov %r0, sp" : "=r"(activeTask->stackPointer));

    // SYSTEM MODE
    // Return to supervisor mode
    asm volatile("mov r12, #0b10011");
    asm volatile("msr cpsr, r12");

    // SUPERVISOR MODE
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
    activeTask->taskState = STATE.READY;

    switch(request) {
        case 2:
            int tid = handleCreate(arg1, arg2);
            activeTask->r0 = tid;
            break;

        case 3:
            int selfTid = handleMyTid();
            activeTask->r0 = selfTid;
            break;

        case 4:
            int parentTid = handleMyParentTid();
            activeTask->r0 = parentTid;
            break;

        case 5:
            break;

        case 6:
            handleExit();
            break;
    }
    
}

int Kernel::handleCreate(int priority, void (*function)()) {

}

int Kernel::handleMyTid() {
    return activeTask->selfTid;
}

int Kernel::handleMyParentTid() {
    return activeTask->parentTid;
}

void Kernel::handleExit() {
    activeTask->taskState = STATE.ZOMBIE;
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