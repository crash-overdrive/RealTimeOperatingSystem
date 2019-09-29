#include "../../include/Kernel.hpp"

typedef void (*funcvoid0_t)();
extern funcvoid0_t __init_array_start, __init_array_end;

Kernel kern;

int main () {
    /*
     * Configure caching to be on. This occurs here instead of in the Kernel because it's the first thing we should do before anything else runs.
     * asm volatile("mcr p15, 0, r1, CRn, CRm, 0"); generically
     * 
     * Bits 2 and 12 set ICache and DCache respectively (instruction and data caches)
     *   To turn on ICache
     *     | 0x800
     *   To turn off ICache
     *     & 0x7FF
     *   To turn on DCache
     *     | 0x2
     *   To turn off DCache
     *     & 0xD
     *   To turn on both
     *     | 0x802
     *   To turn off both
     *     & 0x7FD
     */
    // TODO: Create functions for turning on and off the cache for the purpose of A2, but leave this inlined here so that it is literally the first thing to run
    asm volatile("mrc p15, 0, r1, c1, c0, 0");
    asm volatile("ldr r2, =0x802");
    asm volatile("orr r1, r1, r2");
    asm volatile("mcr p15, 0, r1, c1, c0, 0");

    for (funcvoid0_t* ctr = &__init_array_start; ctr < &__init_array_end; ctr += 1) (*ctr)();

    kern.run();
}
