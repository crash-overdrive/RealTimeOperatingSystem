#define FOREVER for (;;)
#include "../kern/Kernel.cpp"

extern "C" {
	#include <bwio.h>
}

int main () {
    Kernel kern;
    TaskDescriptor *active;
    int request;

    kern.initialize(); // includes starting the first task
    FOREVER {
        active = kern.schedule();
        request = kern.activate(active);
        kern.handle(request);
    }
	bwprintf(COM2, "It works!");

}
