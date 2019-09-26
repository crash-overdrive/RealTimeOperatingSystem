#include "../../include/Kernel.hpp"

void Kernel::initialize() {
    // Setup comm
    uart.setConfig(COM1, BPF8, OFF, ON, OFF);
	uart.setConfig(COM2, BPF8, OFF, OFF, OFF);

    // Setup 0x08, 0x28
    asm volatile("mov r0, #0xe59ff018"); // e59ff018 = ldr pc, [pc, #24]
    asm volatile("str r0, #0x8");
    asm volatile("ldr r0, =kernel_entry");
    asm volatile("str r0, #0x28");

    // TODO: Setup first task
    handleCreate(1, (void *)firstTask);
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
    // TODO: implement me
}

void Kernel::firstTask() {
    int a = 1;
    int b = 2;
    int c = a + b;
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