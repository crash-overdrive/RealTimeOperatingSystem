#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/SensorTrainCourier.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorDiffMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void sensorTrainCourier() {
    int SENSOR = WhoIs("SENSOR");
    int TRAIN = WhoIs("TRAIN");
    int result;

    SensorAttrMessage samsg;
    SensorDiffMessage sdmsg;

    FOREVER {
        // Send sensor attr message to sensor server and wait for sensor diff message
        result = Send(SENSOR, (char *)&samsg, samsg.size(), (char*)&sdmsg, sdmsg.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Sensor->Train Courier - Send to Sensor Server failed\r\n");
        }
        if (sdmsg.mh.type != Constants::MSG::SENSOR_DIFF) {
            bwprintf(COM2, "Sensor->Train Courier - Expected SENSOR_DIFF message but received unexpected message type %d\r\n", sdmsg.mh.type);
        }

        // Send sensor diff message to train server and get sensor attr message
        result = Send(TRAIN, (char*)&sdmsg, sdmsg.size(), (char *)&samsg, samsg.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Sensor->Train Courier - Send to Train Server failed\r\n");
        }
        if (samsg.mh.type != Constants::MSG::SENSOR_ATTR) {
            bwprintf(COM2, "Sensor->Train Courier - Expected SENSOR_ATTR message type but received unexpected message type %d\r\n", samsg.mh.type);
        }
    }
}
