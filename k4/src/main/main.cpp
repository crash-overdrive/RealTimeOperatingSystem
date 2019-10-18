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

    // Turn caching on (it would be nice to move more of the kernel configuration code to this assembly)
    configureKernel();

    // Turn halting on
    *(int *)0x809300c0 = 0xAA; // Unlocks SysSWLock, the Syscon lock
    *(int *)0x80930080 |= 0x1; // Writes SHena bit of DeviceCfg Syscon register

    // Turn interrupts on
    *(int *)(VIC1_IRQ_BASE + IRQ_ENABLE_OFFSET) = TC2UI_MASK | UART2_RX_INTR2_MASK | UART2_TX_INTR2_MASK;
    // *(int *)(VIC1_IRQ_BASE + IRQ_ENABLE_OFFSET) = TC1UI_MASK | TC2UI_MASK;
    *(int *)(VIC2_IRQ_BASE + IRQ_ENABLE_OFFSET) = TC3UI_MASK;

    // Set all interrupts to IRQ
    *(int *)(VIC1_IRQ_BASE + IRQ_SELECT_OFFSET) = 0;
    *(int *)(VIC2_IRQ_BASE + IRQ_SELECT_OFFSET) = 0;

    kern.run();
}
