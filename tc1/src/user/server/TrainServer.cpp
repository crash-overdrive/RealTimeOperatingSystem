#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/TrainMarklinCourier.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/model/Train.hpp"
#include "user/server/TrainServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

TRINDEX TrainServer::getTrainIndex(char number) {
    switch (number) {
        case 1:
            return TRINDEX::T1;
        case 24:
            return TRINDEX::T24;
        case 58:
            return TRINDEX::T58;
        case 74:
            return TRINDEX::T74;
        case 78:
            return TRINDEX::T78;
        default:
            bwprintf(COM2, "Train Server - Request for invalid train index! %d", number);
            return TRINDEX::T1;
    }
}

void TrainServer::queueTrainSpeed(char tr, char s) {
    trbuf[getTrainIndex(tr)].push(TRMessage(tr, s));
}

void TrainServer::queueReverse(char tr) {
    TRINDEX train = getTrainIndex(tr);
    // TODO: calculate timing for slow down/ speed up
    char speed = trains[train].speed;
    trbuf[train].push(TRMessage(tr, (char)0));
    trbuf[train].push(TRMessage(tr, (char)15));
    trbuf[train].push(TRMessage(tr, speed));
}

// Returns the index of the first non-empty train command queue or -1 if all are empty
int TrainServer::queueIndex() {
    for (int i = 0; i < 5; ++i) {
        if (!trbuf[i].empty()) {
            return i;
        }
    }
    return -1;
}

TRMessage TrainServer::popTRMessage(int i) {
    return trbuf[i].pop();
}

void TrainServer::setTrainSpeed(char tr, char s) {
    trains[getTrainIndex(tr)].speed = s;
}

void TrainServer::setTrainSpeed(int train, char s) {
    trains[train].speed = s;
}

void TrainServer::init() {
    trains[T1] = Train(1);
    trains[T24] = Train(24);
    trains[T58] = Train(58);
    trains[T74] = Train(74);
    trains[T78] = Train(78);
    // TODO: init train estimates
    marklinCourier = Create(5, trainMarklinCourier);
    marklinCourierReady = false;
}

void trainServer() {
    RegisterAs("TRAIN");

    TrainServer ts;
    ts.init();

    int result, tid;
    char msg[16];
    MessageHeader *mh = (MessageHeader*)&msg;
    RVMessage *rvmsg = (RVMessage *)&msg;
    TRMessage *trmsg = (TRMessage *)&msg;

    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage errmsg(Constants::MSG::ERR);



    FOREVER {
        result = Receive(&tid, (char*)&msg, 16);
        if(result < 0) {
            // TODO: handle empty message
        }

        if (mh->type == Constants::MSG::RDY) {
            int index = ts.queueIndex();
            if (index < 0) {
                ts.marklinCourierReady = true;
            } else {
                *trmsg = ts.popTRMessage(index);
                ts.setTrainSpeed(index, trmsg->speed);
                Reply(ts.marklinCourier, msg, trmsg->size());
            }
        } else if (mh->type == Constants::MSG::TR) {
            if (ts.marklinCourierReady) {
                ts.setTrainSpeed(trmsg->train, trmsg->speed);
                Reply(ts.marklinCourier, msg, trmsg->size());
                ts.marklinCourierReady = false;
            } else {
                ts.queueTrainSpeed(trmsg->train, trmsg->speed);
            }
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RV) {
            ts.queueReverse(rvmsg->train);
            if (ts.marklinCourierReady) {
                *trmsg = ts.popTRMessage(ts.getTrainIndex(rvmsg->train));
                Reply(ts.marklinCourier, msg, trmsg->size());
                ts.marklinCourierReady = false;
            } else {
                ts.queueReverse(rvmsg->train);
            }
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "Train Server - Unrecognized message type received %d", mh->type);
        }
    }
}
