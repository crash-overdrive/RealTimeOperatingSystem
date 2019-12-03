#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/SensorTrainCourier.hpp"
#include "user/message/SensorMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void sensorGUICourier() {
    int SENSOR = WhoIs("SENSOR");
    int GUI = WhoIs("GUI");
    int result;

    SensorMessage sensormsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // get sensor message from sensor server
        result = Send(SENSOR, (char *)&rdymsg, rdymsg.size(), (char*)&sensormsg, sensormsg.size());
        if (result < 0) {
            bwprintf(COM2, "Sensor->GUI Courier - Send to Sensor Server failed\r\n");
        }
        if (sensormsg.mh.type != Constants::MSG::SENSOR) {
            bwprintf(COM2, "Sensor->GUI Courier - Expected SENSOR message but received unexpected message type %d\r\n", sensormsg.mh.type);
        }

        // Send sensor message to gui server
        result = Send(GUI, (char*)&sensormsg, sensormsg.size(), (char *)&rdymsg, rdymsg.size());
        if (result < 0) {
            bwprintf(COM2, "Sensor->GUI Courier - Send to GUI Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "Sensor->GUI Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
