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
    mrs r12, cpsr
    stmfd sp!, {r12}

    // Move pc, cpsr of active task into lr_svc, spsr_svc
    ldmfd r0!, {r1, r2}
    mov lr, r1
    msr spsr, r2
    
    // ENTER SYSTEM MODE
    msr cpsr_c, #0b11111

    // set the right stack first into sp_usr
    mov sp, r0
    // and then Load registers from user stack
    ldmfd sp!, {r0-r12, lr}

    // ENTER SUPERVISOR MODE
    msr cpsr_c, #0b10011

    // the correct return value for the user task
    // should have already been set by Kernel::activate
 
    // Go back to user mode
    movs pc, lr
    

.globl context_switch_enter
.type context_switch_enter, %function
//returns the stackpointer of the user task which just called swi
context_switch_enter:
    // returns stack Pointer of task which just called SWI 
    // ENTER SYSTEM MODE
    msr cpsr_c, #0b11111

    // Save user state (r0-r12, lr) on user stack  
    // sp is decremented appropriately and saved into the task descriptor
    stmfd sp!, {r0-r12, lr}

    // Save value of user stackPointer in r0 because this is what we will return
    mov r0, sp
    
    // ENTER SUPERVISOR MODE
    msr cpsr_c, #0b10011

    // save pc of the user process in r1, which is gonna be in lr_svc
    mov r1, lr

    // save cpsr of the user process in r2, which is gonna be in spsr_svc
    mrs r2, spsr

    // save lr and cpsr on sp_usr
    stmfd r0!, {r1, r2}

    // Retrieve Kernal State from Kernal Stack
    ldmfd sp!, {r12}
    msr cpsr, r12
    ldmfd sp!, {r4-r12, lr}

    // Jump back to kernel code in Kernel.cpp
    mov pc, lr
