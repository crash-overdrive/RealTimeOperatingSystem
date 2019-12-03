#ifndef SENSOR_ATTR_MESSAGE
#define SENSOR_ATTR_MESSAGE

#include "Constants.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/model/Sensor.hpp"

class SensorAttrMessage {
public:
    MessageHeader mh;
    int count;
    SensorAttr sensorAttrs[5];

    SensorAttrMessage();

    int size();
    int maxSize();
};

#endif
