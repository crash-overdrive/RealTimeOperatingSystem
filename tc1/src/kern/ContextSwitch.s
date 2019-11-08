.globl kernelExit
.type kernelExit, %function
// takes in stackPointer of the user task to be activated, with correct return value set in sp[r0]
// returns to user Task to be activated with value of r0 set correctly
kernelExit:

    // BEGIN IN SUPERVISOR MODE
    // r0 is stackPointer of user task to be activated

    // store kernel state on kernel stack
    // r4-r12, lr, cpsr
    stmfd sp!, {r4-r12, lr}

    // Pop off the sentinal value for IRQ
    ldmfd r0!, {r3}
    // Move pc, cpsr of active task into lr_svc, spsr_svc
    ldmfd r0!, {r1, r2}
    mov lr, r1
    msr spsr, r2

    // ENTER SYSTEM MODE
    msr cpsr_c, #0b10011111

    // set the right stack first into sp_usr
    mov sp, r0
    // and then Load registers from user stack
    ldmfd sp!, {r0-r12, lr}

    // ENTER SUPERVISOR MODE
    msr cpsr_c, #0b10010011

    // the correct return value for the user task
    // should have already been set by Kernel::activate

    // Go back to user mode
    movs pc, lr


.globl context_switch_enter
.type context_switch_enter, %function
// Returns the stack ointer of the user task which just called SWI
context_switch_enter:
    // ENTER SYSTEM MODE
    msr cpsr_c, #0b10011111

    // Save user state (r0-r12, lr) on user stack
    // sp is decremented appropriately and saved into the task descriptor
    stmfd sp!, {r0-r12, lr}

    // Save value of user stackPointer in r0 because this is what we will return
    mov r0, sp

    // ENTER SUPERVISOR MODE
    msr cpsr_c, #0b10010011

    // save pc of the user process in r1, which is gonna be in lr_svc
    mov r1, lr

    // save cpsr of the user process in r2, which is gonna be in spsr_svc
    mrs r2, spsr

    // save lr and cpsr on sp_usr
    stmfd r0!, {r1, r2}

    // Store sentinal value for SWI
    mov r3, #0
    stmfd r0!, {r3}

    // Retrieve Kernal State from Kernal Stack
    ldmfd sp!, {r4-r12, pc}

.globl handle_interrupt
.type handle_interrupt, %function
// Returns stack pointer of user task which was interrupted
handle_interrupt:
    // ENTER SYSTEM MODE
    msr cpsr_c, #0b11011111

    // Save user state (r0-r12, lr) on user stack
    // sp is decremented appropriately and saved into the task descriptor
    stmfd sp!, {r0-r12, lr}

    // Save value of user stackPointer in r0 because this is what we will return
    mov r0, sp

    // ENTER INTERRUPT MODE
    msr cpsr_c, #0b11010010

    // save pc of the user process in r1, which is gonna be in lr_irq
    sub r1, lr, #4

    // save cpsr of the user process in r2, which is gonna be in spsr_irq
    mrs r2, spsr

    // save lr and cpsr on sp_usr
    stmfd r0!, {r1, r2}

    // Store sentinal value for IRQ
    mov r3, #1
    stmfd r0!, {r3}

    // ENTER SUPERVISOR MODE
    msr cpsr_c, #0b11010011

    // Retrieve Kernal State from Kernal Stack
    ldmfd sp!, {r4-r12, pc}