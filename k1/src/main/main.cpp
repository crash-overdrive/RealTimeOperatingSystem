#include "../../include/Kernel.hpp"
#include <cstring>
#include <cstdint>

typedef void (*funcvoid0_t)();
extern funcvoid0_t __init_array_start, __init_array_end;

Kernel kern;

int main () {

    for (funcvoid0_t* ctr = &__init_array_start; ctr < &__init_array_end; ctr += 1) (*ctr)();

    kern.run();
}
