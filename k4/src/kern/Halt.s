.globl Halt
.type Halt, %function
// takes in no value
// puts system in Idle mode
// when returns r0 is the idleTime
Halt:
    ldr r0, .HaltAddress
    ldr r0, [r0]
    mov pc, lr

.HaltAddress:
    .word	-2137849848
