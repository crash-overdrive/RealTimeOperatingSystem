.globl configureKernel
.type configureKernel, %function

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
configureKernel:
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #(0x1 << 12)
    orr r0, r0, #(0x1 << 2)
    mcr p15, 0, r0, c1, c0, 0

    mov pc, lr
