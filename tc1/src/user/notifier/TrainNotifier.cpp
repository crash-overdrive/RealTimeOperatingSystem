#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/notifier/TrainNotifier.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void trainNotifier() {
    int CLOCK = WhoIs("CLOCK SERVER");
    int TRAIN = WhoIs("TRAIN");

    int result;

    ThinMessage tickmsg(Constants::MSG::TICK);
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Can also implement this as listening for interrupt on Timer1
        Delay(CLOCK, 1);
        result = Send(TRAIN, (char *)&tickmsg, tickmsg.size(), (char *)&rdymsg, rdymsg.size());

        if (result < 0) {
            bwprintf(COM2, "Train Notifier - Send to Train Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Train Notifier - Expected RDY message type but received %d\r\n", rdymsg.mh.type);
        }
    }
}
