#ifndef TRAIN_SERVER_HPP
#define TRAIN_SERVER_HPP

#include "data-structures/RingBuffer.hpp"
#include "user/model/Track.hpp"
#include "user/model/Train.hpp"
#include "user/message/LocationMessage.hpp"
#include "user/message/SensorDiffMessage.hpp"
#include "user/message/SensorPredMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/message/TrainMessage.hpp"

void trainServer();

class TrainServer {
public:
    char msg[128];
    SensorAttrMessage samsg;
    SensorDiffMessage *sdmsg = (SensorDiffMessage *)&msg;
    SensorPredMessage *spmsg = (SensorPredMessage *)&msg;
    TrainMessage trainmsg;
    LocationMessage locmsg;

    Train trains[5];
    DataStructures::RingBuffer<TRMessage, 32> trbuf[5];
    int marklinCourier, navCourier, guiCourier, locNavCourier, locGUICourier, notifier;
    bool marklinCourierReady, navCourierReady, guiCourierReady, locNavCourierReady, locGUICourierReady;
    int CLOCK, currtime, prevtime, tickCount;

    Track track;

    void init();
    TRINDEX getTrainIndex(char number);
    void queueTrainSpeed(char tr, char s);
    void queueReverse(char tr);
    int queueIndex();
    void setTrainSpeed(char tr, char s);
    void setTrainSpeed(int train, char s);
    TRMessage popTRMessage(int i);
    void attributeSensors();
    void updatePredictions();
    void sendGUI();
    bool updated();
    void updateLocation();
    int getDirection(int i);
    void sendLocation();
};

#endif
