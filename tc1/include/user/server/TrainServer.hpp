#ifndef TRAIN_SERVER_HPP
#define TRAIN_SERVER_HPP

#include "data-structures/RingBuffer.hpp"
#include "user/model/Train.hpp"
#include "user/message/TRMessage.hpp"

void trainServer();

enum TRINDEX {
    T1 = 0,
    T24,
    T58,
    T74,
    T78,
};

class TrainServer {
public:
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
};

#endif
