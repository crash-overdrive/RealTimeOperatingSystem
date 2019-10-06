#include "user/client/IdleClient.hpp"
#include "io/bwio.hpp"

#define FOREVER for(;;)

void idleClient() {
    bwprintf(COM2, "Created idle client\n\r");
    FOREVER {
        // bwprintf(COM2, "I am running\n\r");
    }
}