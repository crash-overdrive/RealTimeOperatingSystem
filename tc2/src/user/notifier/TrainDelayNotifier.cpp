#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/message/DelayMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void trainDelayNotifier() {
    int TRAIN = WhoIs("TRAIN");
    int CLOCK = WhoIs("CLOCK");

    int result;

    DelayMessage delaymsg;

    FOREVER {
        // Notify train server when ready to receive next delay
        result = Send(TRAIN, (char *)&delaymsg, delaymsg.size(), (char *)&delaymsg, delaymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Train Delay Notifier - Send to Train Server failed\r\n");
        }
        if (delaymsg.mh.type != Constants::MSG::DELAY) {
            bwprintf(COM2, "Train Delay Notifier - Expected DELAY message type but received %d\r\n", delaymsg.mh.type);
        }

        // Delay for requested amount of time
        Delay(CLOCK, delaymsg.delay);
    }
}
