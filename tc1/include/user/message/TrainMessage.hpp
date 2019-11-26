#ifndef TRAIN_MESSAGE
#define TRAIN_MESSAGE

#include "MessageHeader.hpp"
#include "user/model/Sensor.hpp"
#include "user/model/TrainInfo.hpp"

class TrainMessage {
public:
    MessageHeader mh;

    int count;
    TrainInfo trainInfo[5];

    TrainMessage();

    int size();
    int maxSize();
};

#endif
