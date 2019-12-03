#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/SwitchMarklinCourier.hpp"
#include "user/message/TRMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void trainMarklinCourier() {
    int TRAIN = WhoIs("TRAIN");
    int MARKLIN = WhoIs("MARKLIN");
    int result;

    TRMessage trmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get switch command from switch server
        result = Send(TRAIN, (char *)&rdymsg, rdymsg.size(), (char*)&trmsg, trmsg.size());
        if (result < 0) {
            bwprintf(COM2, "Train->Marklin Courier - Send to Train Server failed\r\n");
        }
        if (trmsg.mh.type != Constants::MSG::TR) {
            bwprintf(COM2, "Train->Marklin Courier - Expected TR message type but received unexpected message type %d\r\n", trmsg.mh.type);
        }

        // Send switch command to marklin server
        result = Send(MARKLIN, (char*)&trmsg, trmsg.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Train->Marklin Courier - Send to Marklin Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Train->Marklin Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
