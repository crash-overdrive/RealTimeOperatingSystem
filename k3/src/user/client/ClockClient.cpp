#include "user/client/ClockClient.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "Constants.hpp"

void clockClient() {

    // Send();
    int clockServerTid = WhoIs("wCLOCK SERVER");
    for (int i = 1; i <= 10; ++i) {
        AwaitEvent(Constants::EVENTS::TIMER_INTERRUPT);
        int time = Delay(clockServerTid, 5);
        bwprintf(COM2, "Clock Client - Got time: %d\n\r", time);

        time = DelayUntil(clockServerTid, 900);
        bwprintf(COM2, "Clock Client - Got time: %d\n\r", time);

        time = Time(clockServerTid);
        bwprintf(COM2, "Clock Client - Got time: %d\n\r", time);
    }
    Exit();
}