#ifndef TRAIN_SERVER_HPP
#define TRAIN_SERVER_HPP

#include "data-structures/RingBuffer.hpp"
#include "user/model/Track.hpp"
#include "user/model/Train.hpp"
#include "user/message/DelayMessage.hpp"
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
    TRMessage *trmsg = (TRMessage *)&msg;
    TrainMessage trainmsg;
    LocationMessage locmsg;
    DelayMessage delaymsg;

    Train trains[5];
    int delayNotifiers[5];
    DataStructures::RingBuffer<TRMessage, 32> trbuf[5];
    int marklinCourier, navCourier, guiCourier, locNavCourier, locGUICourier, tickNotifier;
    bool marklinCourierReady, navCourierReady, guiCourierReady, locNavCourierReady, locGUICourierReady;
    int CLOCK, currtime, prevtime, tickCount;
    int trainVelocity[5] = {0};

    Track track;

    void init();
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
    int getNextSensorDistance(int i);
    int getDelayNotifierIndex(int tid);
    void sendMarklin();
};

#endif
