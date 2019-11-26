#ifndef TRAIN_MESSAGE
#define TRAIN_MESSAGE

#include "MessageHeader.hpp"
#include "user/model/Sensor.hpp"

class TrainMessage {
public:
    MessageHeader mh;

    Sensor next, prev;
    int predTime, predDist, realTime, realDist;
    char train;

    TrainMessage();

    int size();
};

#endif
