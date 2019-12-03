#ifndef SENSOR_PRED_MESSAGE
#define SENSOR_PRED_MESSAGE

#include "Constants.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/model/Sensor.hpp"

class SensorPredMessage {
public:
    MessageHeader mh;
    int count;
    SensorPred predictions[6];

    SensorPredMessage();

    int size();
    int maxSize();
};

#endif
