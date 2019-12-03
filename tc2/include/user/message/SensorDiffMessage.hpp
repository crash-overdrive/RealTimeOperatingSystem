#ifndef SENSOR_DIFF_MESSAGE
#define SENSOR_DIFF_MESSAGE

#include "Constants.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/model/Sensor.hpp"

class SensorDiffMessage {
public:
    MessageHeader mh;
    int count;
    Sensor sensors[80];

    SensorDiffMessage();

    int size();
    int maxSize();
};

#endif
