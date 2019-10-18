.globl Halt
.type Halt, %function
// takes in no value
// puts system in Idle mode
// when returns r0 is the idleTime
Halt:
    ldr r0, .HaltAddress
    ldr r0, [r0] // this puts the system in Halt Mode
    // 2 NOP's because once the Interrupt is discovered, 2 extra instructions are executed
    NOP
    NOP
    // System goes to 0x18 from here
    // Comes back here and r0 is now correctly set with idleTime
    mov pc, lr

.HaltAddress:
    .word	-2137849848
