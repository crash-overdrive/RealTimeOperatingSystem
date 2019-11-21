#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/SwitchMarklinCourier.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorPredMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void trainNavCourier() {
    int TRAIN = WhoIs("TRAIN");
    int NAV = WhoIs("NAV");
    int result;

    SensorAttrMessage samsg;
    SensorPredMessage spmsg;

    FOREVER {
        // Get sensor predictions from nav server
        result = Send(NAV, (char*)&samsg, samsg.size(), (char *)&spmsg, spmsg.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Train->Nav Courier - Send to Nav Server failed\r\n");
        }
        if (spmsg.mh.type != Constants::MSG::SENSOR_PRED) {
            bwprintf(COM2, "Train->Nav Courier - Expected RDY message type but received unexpected message type %d\r\n", spmsg.mh.type);
        }

        // Get sensor attributions from train server
        result = Send(TRAIN, (char *)&spmsg, spmsg.size(), (char*)&samsg, samsg.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Train->Nav Courier - Send to Train Server failed\r\n");
        }
        if (samsg.mh.type != Constants::MSG::SENSOR_ATTR) {
            bwprintf(COM2, "Train->Nav Courier - Expected SENSOR_ATTR message type but received unexpected message type %d\r\n", samsg.mh.type);
        }
    }
}
