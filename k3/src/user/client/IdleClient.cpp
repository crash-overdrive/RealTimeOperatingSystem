#include "user/syscall/UserSyscall.hpp"
#include "user/client/IdleClient.hpp"
#include "io/bwio.hpp"

#define FOREVER for(;;)

struct diagnostics {
    int idlePercent;
};

void displayIdle(int idlePercent) {
    // TODO: implement an idle display
    if (idlePercent < 50000) {
        bwprintf(COM2, "   RED: Idle percent %d\n\r", idlePercent);
    } else if (idlePercent < 80000) {
        bwprintf(COM2, "YELLOW: Idle percent %d\n\r", idlePercent);
    } else {
        bwprintf(COM2, " GREEN: Idle percent %d\n\r", idlePercent);
    }
}

void diagnosis(struct diagnostics *diags) {
    displayIdle(diags->idlePercent);
}

void idleClient() {
    struct diagnostics diags;
    bwprintf(COM2, "Created idle client\n\r");
    FOREVER {
        diagnosis(&diags);
        bwprintf(COM2, "Idle client halting\n\r");
        diags.idlePercent = Halt();
    }
}