#include <cstring>

#include "Constants.hpp"
#include "Util.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/TrainGUICourier.hpp"
#include "user/courier/TrainMarklinCourier.hpp"
#include "user/courier/TrainNavCourier.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorDiffMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/message/TrainMessage.hpp"
#include "user/notifier/TrainNotifier.hpp"
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

void TrainServer::attributeSensors() {
    for (int i = 0; i < sdmsg->count; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (sdmsg->sensors[i] == trains[j].nextSensor[0]) {
                // TODO: handle updates determined by sensor attribution
                samsg.sensorAttrs[samsg.count].sensor = sdmsg->sensors[i];
                samsg.sensorAttrs[samsg.count].train = trains[j].number;
                samsg.count++;
                trains[j].nextSensor[0].bank = 0;
                trains[j].nextSensor[0].number = 0;
                break;
            } else if (sdmsg->sensors[i] == trains[j].nextSensor[1]) {
                // TODO: handle updates determined by sensor attribution
                samsg.sensorAttrs[samsg.count].sensor = sdmsg->sensors[i];
                samsg.sensorAttrs[samsg.count].train = trains[j].number;
                samsg.count++;
                trains[j].nextSensor[0].bank = 0;
                trains[j].nextSensor[0].number = 0;
                trains[j].nextSensor[1].bank = 0;
                trains[j].nextSensor[1].number = 0;
                break;
            }
        }
    }
}

void TrainServer::updatePredictions() {
    // TODO: determine what (if any) updates need to happen here
    for (int i = 0; i < spmsg->count; ++i) {
        int train = spmsg->predictions[i].train;
        int index = Train::getTrainIndex(train);
        trains[index].nextSensor[0] = spmsg->predictions[i].nextSensor[0];
        trains[index].nextSensor[1] = spmsg->predictions[i].nextSensor[1];
    }
}

bool TrainServer::updated() {
    for (int i = 0; i < 5; ++i) {
        if (trains[i].updated) {
            return true;
        }
    }
    return false;
}

void TrainServer::updateLocation() {
    prevtime = currtime;
    currtime = Time(CLOCK);
    int delta = currtime - prevtime; // Number of ticks, should always be 1
    ASSERT(delta == 1);
    for (int i = 0; i < 5; ++i) {
        if (trains[i].speed != 0) {
            // Calculate distance travelled since last tick
            // TODO: improve logic to account for acceleration
            int dist = trains[i].vel[trains[i].speed];

            // Add train to location message
            locmsg.locationInfo[locmsg.count].train = trains[i].number;
            locmsg.locationInfo[locmsg.count].location = trains[i].location;
            locmsg.count++;
        }
        // bwprintf(COM2, "(%d %d)", dist, trains[i].speed);
    }

    // Reply(locNavCourier, (char *)&locmsg, locmsg.size());
    // Reply(locGUICourier, (char *)&locmsg, locmsg.size());
    locmsg.count = 0;
}

void TrainServer::sendGUI() {
    // TODO: need to know when train information has been updated
    if (updated() && guiCourierReady) {
        // TODO: refactor this to send as many updates as needed, and all relevant information
        trainmsg.trainInfo[0].prev = trainmsg.trainInfo[0].next;
        trainmsg.trainInfo[0].next = trains[1].nextSensor[0];
        // Faking a sensor
        trainmsg.trainInfo[0].prev = Sensor('A', 15);
        trainmsg.trainInfo[0].next = Sensor('E', 10);
        trainmsg.trainInfo[0].predictedDist = 100;
        trainmsg.trainInfo[0].predictedTime = 29;
        trainmsg.trainInfo[0].realDist = 1200;
        trainmsg.trainInfo[0].realTime = 26;
        trainmsg.trainInfo[0].number = 24;
        trainmsg.count = 1;
        guiCourierReady = false;
        Reply(guiCourier, (char*)&trainmsg, trainmsg.size());
        trains[0].updated = false;
    }
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
    navCourier = Create(5, trainNavCourier);
    navCourierReady = false;
    guiCourier = Create(8, trainGUICourier);
    guiCourierReady = false;
    notifier = Create(5, trainNotifier);

    trains[0].updated = true;

    // Set the speed measurements for the trains
    int velocities[5][15] = {
        { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
        { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
        { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
        { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
        { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
    };
    for (int i = 0; i < 5; ++i) {
        memcpy(trains[i].vel, velocities[i], 15*sizeof(int));
    }

    prevtime = 0;
    currtime = 0;

    trains[1].speed = 14;
    trains[1].location.landmark = 4;
    trains[1].location.offset = 1000;
    // trains[1].speed = 5;
    // trains[2].speed = 6;
    // trains[3].speed = 13;
    // trains[4].speed = 14;

    CLOCK = WhoIs("CLOCK SERVER");
}

void trainServer() {
    RegisterAs("TRAIN");

    TrainServer ts;
    ts.init();

    int result, tid;
    MessageHeader *mh = (MessageHeader*)&ts.msg;
    RVMessage *rvmsg = (RVMessage *)&ts.msg;
    TRMessage *trmsg = (TRMessage *)&ts.msg;

    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage errmsg(Constants::MSG::ERR);

    FOREVER {
        result = Receive(&tid, (char*)&ts.msg, 128);
        if(result < 0) {
            // TODO: handle empty message
        }

        if (mh->type == Constants::MSG::TICK) {
            ts.updateLocation();
            Reply(tid, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RDY) {
            int index = ts.queueIndex();
            if (tid == ts.marklinCourier) {
                if (index < 0) {
                    ts.marklinCourierReady = true;
                } else {
                    *trmsg = ts.popTRMessage(index);
                    ts.setTrainSpeed(index, trmsg->speed);
                    Reply(ts.marklinCourier, ts.msg, trmsg->size());
                }
            } else if (tid == ts.guiCourier) {
                ts.guiCourierReady = true;
                ts.sendGUI();
            } else {
                bwprintf(COM2, "Train Server - Unexpected ready message!");
            }
        } else if (mh->type == Constants::MSG::TR) {
            if (ts.marklinCourierReady) {
                ts.setTrainSpeed(trmsg->train, trmsg->speed);
                Reply(ts.marklinCourier, ts.msg, trmsg->size());
                ts.marklinCourierReady = false;
            } else {
                ts.queueTrainSpeed(trmsg->train, trmsg->speed);
            }
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RV) {
            ts.queueReverse(rvmsg->train);
            if (ts.marklinCourierReady) {
                *trmsg = ts.popTRMessage(ts.getTrainIndex(rvmsg->train));
                Reply(ts.marklinCourier, ts.msg, trmsg->size());
                ts.marklinCourierReady = false;
            } else {
                ts.queueReverse(rvmsg->train);
            }
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::SENSOR_DIFF) {
            ts.attributeSensors();
            if (ts.samsg.count > 0) {
                Reply(ts.navCourier, (char *)&ts.samsg, ts.samsg.size());
                ts.navCourierReady = false;
            }
            Reply(tid, (char *)&ts.samsg, ts.samsg.size());
            ts.samsg.count = 0; // Reset the state of the attribution
        } else if (mh->type == Constants::MSG::SENSOR_PRED) {
            // If it's an empty message there's nothing to update (This occurs when receiving the first spmsg from the courier)
            if (ts.spmsg->count == 0) {
                ts.navCourierReady = true;
                continue;
            }
            ts.updatePredictions();
            if (tid == ts.navCourier) {
                // This is updated, but since this communication chain is dependent, do not actually need to track this
                ts.navCourierReady = true;
            } else {
                Reply(tid, (char*)&rdymsg, rdymsg.size());
            }
            ts.sendGUI(); // Send updated information to GUI Server
        } else {
            bwprintf(COM2, "Train Server - Unrecognized message type received %d", mh->type);
        }
    }
}
