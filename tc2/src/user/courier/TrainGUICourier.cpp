#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TrainGUICourier.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TrainMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void trainGUICourier() {
    int TRAIN = WhoIs("TRAIN");
    int GUI = WhoIs("GUI");
    int result;

    TrainMessage trainmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get update from the train server
        result = Send(TRAIN, (char *)&rdymsg, rdymsg.size(), (char*)&trainmsg, trainmsg.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Train->GUI Courier - Send to Train Server failed\r\n");
        }
        if (trainmsg.mh.type != Constants::MSG::TRAIN) {
            bwprintf(COM2, "Train->GUI Courier - Expected TRAIN message type but received unexpected message type %d\r\n", trainmsg.mh.type);
        }

        // Send update to the gui server
        result = Send(GUI, (char*)&trainmsg, trainmsg.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Train->GUI Courier - Send to GUI Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Train->GUI Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
