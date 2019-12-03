#ifndef SENSOR_MESSAGE
#define SENSOR_MESSAGE

#include "MessageHeader.hpp"
#include "user/model/Sensor.hpp"

class SensorMessage {
public:
    MessageHeader mh;

    SensorAttr sensorData[10];

    SensorMessage();

    int size();
};

#endif
