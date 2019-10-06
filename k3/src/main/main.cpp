#include "kern/Kernel.hpp"
#include "io/ep9302.h"
#include "io/bwio.hpp"

typedef void (*funcvoid0_t)();
extern funcvoid0_t __init_array_start, __init_array_end;

Kernel kern;

extern "C" void configureKernel();

int main () {
    // Shash said this is something to do with linker or something
    for (funcvoid0_t* ctr = &__init_array_start; ctr < &__init_array_end; ctr += 1) (*ctr)();

    configureKernel();

    // Turn interrupts on
    *(int *)(VIC1_IRQ_BASE + IRQ_ENABLE_OFFSET) = TC1UI_MASK | TC2UI_MASK;
    *(int *)(VIC2_IRQ_BASE + IRQ_ENABLE_OFFSET) = TC3UI_MASK;

    

    // Turn on interupts
    // asm volatile("mrs r0, cpsr");
    // asm volatile("orr r0, #0x80");
    // asm volatile("msr cpsr_c, r0");

    kern.run();
}
