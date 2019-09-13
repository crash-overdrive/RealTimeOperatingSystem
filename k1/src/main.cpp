#define FOREVER for (;;)

int main (void) {
    initialize(); // includes starting the first task
    FOREVER {
        active = schedule();
        request = activate(active);
        handle(request)
    }
}
