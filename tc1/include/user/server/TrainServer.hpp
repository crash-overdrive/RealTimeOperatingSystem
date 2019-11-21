#ifndef TRAIN_SERVER_HPP
#define TRAIN_SERVER_HPP

#include "data-structures/RingBuffer.hpp"
#include "user/model/Train.hpp"
#include "user/message/TRMessage.hpp"

void trainServer();

class TrainServer {
public:
    char msg[128];
    SensorAttrMessage samsg;
    SensorDiffMessage *sdmsg = (SensorDiffMessage *)&msg;

    Train trains[5];
    DataStructures::RingBuffer<TRMessage, 32> trbuf[5];
    int marklinCourier;
    bool marklinCourierReady;

    void init();
    TRINDEX getTrainIndex(char number);
    void queueTrainSpeed(char tr, char s);
    void queueReverse(char tr);
    int queueIndex();
    void setTrainSpeed(char tr, char s);
    void setTrainSpeed(int train, char s);
    TRMessage popTRMessage(int i);
    void attributeSensors();
    void sendSensorAttr();
};

#endif
