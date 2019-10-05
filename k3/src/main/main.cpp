#include "kern/Kernel.hpp"
#include "io/ep9302.h"
#include "io/bwio.hpp"

typedef void (*funcvoid0_t)();
extern funcvoid0_t __init_array_start, __init_array_end;

Kernel kern;

extern "C" void configureKernel();

// extern "C" void print();

// // void print(int val) {
// //     bwprintf(COM2, "PRINT %d\r\n", val);
// // }

// void print() {
//     bwprintf(COM2, "PRINT\r\n");
// }

int main () {
    // Shash said this is something to do with linker or something
    for (funcvoid0_t* ctr = &__init_array_start; ctr < &__init_array_end; ctr += 1) (*ctr)();

    configureKernel();

    // Turn interrupts on
    *(int *)(VIC1_IRQ_BASE + IRQ_ENABLE_OFFSET) = TC1UI_MASK | TC2UI_MASK;
    *(int *)(VIC2_IRQ_BASE + IRQ_ENABLE_OFFSET) = TC3UI_MASK;

    /*
     * Configure caching to be on. This occurs here instead of in the Kernel because it's the first thing we should do before anything else runs.
     * asm volatile("mcr p15, 0, r1, CRn, CRm, 0"); generically
     * 
     * Bits 2 and 12 set ICache and DCache respectively (instruction and data caches)
     * Bits start from 0.... :( tricky tricky
     *   To turn on ICache
     *     | 0x800 
     *   To turn off ICache
     *     & 0x7FF
     *   To turn on DCache
     *     | 0x2
     *   To turn off DCache
     *     & 0xD
     *   To turn on both
     *     | 0x804
     *   To turn off both
     *     & 0x7FD
     */
    // TODO: Create functions for turning on and off the cache for the purpose of A2, but leave this inlined here so that it is literally the first thing to run
    // asm volatile("mrc p15, 0, r0, c1, c0, 0");
    // asm volatile("orr r0, r0, #(0x1 << 12)");
    // asm volatile("orr r0, r0, #(0x1 << 2)");
    // asm volatile("mcr p15, 0, r0, c1, c0, 0");


    // Turn on interupts
    // asm volatile("mrs r0, cpsr");
    // asm volatile("orr r0, #0x80");
    // asm volatile("msr cpsr_c, r0");

    kern.run();
}
