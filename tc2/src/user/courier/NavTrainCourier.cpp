#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/NavTrainCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorPredMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void navTrainCourier() {
    int TRAIN = WhoIs("TRAIN");
    int NAV = WhoIs("NAV");
    int result;

    ThinMessage rdymsg(Constants::MSG::RDY);
    SensorPredMessage spmsg;

    FOREVER {
        // Get sensor predictions from nav server
        result = Send(NAV, (char*)&rdymsg, rdymsg.size(), (char *)&spmsg, spmsg.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Nav->Train Courier - Send to Nav Server failed\r\n");
        }
        if (spmsg.mh.type != Constants::MSG::SENSOR_PRED) {
            bwprintf(COM2, "Nav->Train Courier - Expected SENSOR_PRED message type but received unexpected message type %d\r\n", spmsg.mh.type);
        }

        // Get sensor attribution or rdy from train server
        result = Send(TRAIN, (char *)&spmsg, spmsg.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Nav->Train Courier - Send to Train Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Nav->Train Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
