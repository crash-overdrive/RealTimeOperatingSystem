#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/MarklinSensorCourier.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void marklinSensorCourier() {
    int MARKLIN = WhoIs("MARKLIN");
    int SENSOR = WhoIs("SENSOR");
    int result;

    TextMessage textmsg;
    ThinMessage request(Constants::MSG::REQUEST);

    // Breaking convention a bit for our system, the Marklin to Sensor courier sends to the Marklin server first even though the Marklin server created it
    FOREVER {
        // Get get sensor data from marklin server
        result = Send(MARKLIN, (char *)&request, request.size(), (char*)&textmsg, textmsg.size());
        if (result < 0) {
            bwprintf(COM2, "Marklin->Sensor Courier - Send to Marklin Server failed\r\n");
        }
        if (textmsg.mh.type != Constants::MSG::TEXT) {
            bwprintf(COM2, "Marklin->Sensor Courier - Expected TEXT message type but received unexpected message type %d\r\n", textmsg.mh.type);
        }

        // Send sensor data to sensor server
        result = Send(SENSOR, (char*)&textmsg, textmsg.size(), (char *)&request, request.size());
        if (result < 0) {
            bwprintf(COM2, "Marklin->Sensor Courier - Send to Sensor Server failed\r\n");
        }
        if (request.mh.type != Constants::MSG::REQUEST) {
            bwprintf(COM2, "Marklin->Sensor Courier - Expected REQUEST message type but received unexpected message type %d\r\n", request.mh.type);
        }
    }
}
