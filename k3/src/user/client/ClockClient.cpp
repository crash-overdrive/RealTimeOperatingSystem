#include "user/client/ClockClient.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "Constants.hpp"

void clockClient() {

    // Send();
    int clockServerTid = WhoIs("wCLOCK SERVER");
    for (int i = 1; i <= 10; ++i) {
        AwaitEvent(Constants::TIMER_INTERRUPT);
        int time = Time(clockServerTid);
        bwprintf(COM2, "Clock Client - Got time: %d\n\r", time);
    }        
    Exit();
}