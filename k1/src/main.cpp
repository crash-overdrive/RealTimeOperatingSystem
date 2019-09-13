#define FOREVER for (;;)

extern "C" {
	#include <bwio.h>
}
int main () {
    // initialize(); // includes starting the first task
    // FOREVER {
    //     active = schedule();
    //     request = activate(active);
    //     handle(request)
    // }
	bwprintf(COM2, "It works!");

}
