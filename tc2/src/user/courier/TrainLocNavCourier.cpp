#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TrainLocNavCourier.hpp"
#include "user/message/LocationMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void trainLocNavCourier() {
    int TRAIN = WhoIs("TRAIN");
    int NAV = WhoIs("NAV");
    int result;

    LocationMessage locmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get train location update from train server
        result = Send(TRAIN, (char *)&rdymsg, rdymsg.size(), (char*)&locmsg, locmsg.maxSize());
        if (result < 0) {
            bwprintf(COM2, "TrainLoc->Nav Courier - Send to Train Server failed\r\n");
        }
        if (locmsg.mh.type != Constants::MSG::LOCATION) {
            bwprintf(COM2, "TrainLoc->Nav Courier - Expected LOCATION message type but received unexpected message type %d\r\n", locmsg.mh.type);
        }

        // Send train location update to navigation server
        result = Send(NAV, (char*)&locmsg, locmsg.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "TrainLoc->Nav Courier - Send to NAV Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "TrainLoc->Nav Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
