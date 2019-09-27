.globl context_switch_entry
context_switch_entry:
    // Change to system mode
    msr cpsr_c, #0b11111

    // SYSTEM MODE
    // Save user state (r4-r11, lr) on user stack  
    // r12 saved by caller
    // sp, pc should not be saved on the stack
    // sp is decremented appropriately and saved into the task descriptor
    // move stack pointer into r5
    stmfd sp!, {r4-r11, lr}
    mov r5, sp

    // SYSTEM MODE
    // Return to supervisor mode
    msr cpsr_c, #0b10011

    // SUPERVISOR MODE
    // Saving cspr of user mode (which is spsr_svc) into task descriptor
    mrs r6, spsr

    // SUPERVISOR MODE
    // Get Request type, save into r4
    ldr r4, [lr, #-4]
    and r4, #0xff

    // save r0-r6
    ldr r12, =reg0
    str r0, [r12]

    ldr r12, =reg1
    str r1, [r12]

    ldr r12, =reg2
    str r2, [r12]

    ldr r12, =reg3
    str r3, [r12]

    ldr r12, =reg4
    str r4, [r12]

    ldr r12, =reg5
    str r5, [r12]

    ldr r12, =reg6
    str r6, [r12]


    // SUPERVISOR MODE
    // Retrieve Kernal State from Kernal Stack
    ldmfd sp!, {r12}
    msr cpsr, r12
    ldmfd sp!, {r0-r12, lr}


    // exit 
    b kernel_entry


.globl context_switch_exit
context_switch_exit:

    // SUPERVISOR MODE
    // store kernel state on kernel stack
    // r0-r12, lr, cpsr
    stmfd sp!, {r0-r12, lr}
    mrs r12, cpsr
    stmfd sp!, {r12}

    // SUPERVISOR MODE
    // Move cpsr of active task into spsr_svc
    ldr r12, =reg0
    ldr r12, [r12]
    msr spsr, r12
    

    // SUPERVISOR MODE
    // Move the next pc into lr_svc
    ldr r12, =reg1
    ldr lr, [r12]

    // SUPERVISOR MODE
    // Set return value by overwriting r0
    ldr r12, =reg2
    ldr r0, [r12]
    
    // SUPERVISOR MODE
    // Change to system mode
    msr cpsr_c, #0b11111

    // SYSTEM MODE
    // set the right stack first into sp_usr
    ldr r12, =reg3
    ldr sp, [r12]

    // SYSTEM MODE
    // and then Load registers from user stack
    ldmfd sp!, {r4-r11, lr}

    // SYSTEM MODE
    // Return to supervisor mode
    msr cpsr_c, #0b10011
 
    // SUPERVISOR MODE
    // Go back to user mode
    movs pc, lr